#!/usr/bin/env bash
set -CeEuxo pipefail

mkdir build-system
cd build-system
prefix="${PWD}/qemu"
case "${RUNNER_OS}" in
  Linux)
    host_args=(
      --static
      --enable-pie
      --enable-kvm
    )
    ;;
  macOS)
    host_args=(
      --enable-hvf
      -Dprefer_static=true
    )
    ;;
  Windows)
    # UCRT64's GLib links libintl, which needs libiconv in static builds.
    glib_libs=$(pkg-config --static --libs glib-2.0)
    if [[ " ${glib_libs} " != *' -liconv '* ]]; then
      sed -i '/^Libs\.private:/ s/$/ -liconv/' "${MINGW_PREFIX}/lib/pkgconfig/glib-2.0.pc"
    fi
    prefix=$(cygpath -am "${prefix}")
    host_args=(
      --static
      --enable-whpx
      --disable-dsound
    )
    ;;
esac

# Refs: https://github.com/cross-platform-actions/resources/blob/master/ci.rb
# Keep pixman for emulated devices and boards that depend on it.
../configure \
  --without-default-features \
  --enable-system \
  --enable-tcg \
  --enable-fdt=internal \
  --enable-pixman \
  --enable-slirp \
  --enable-stack-protector \
  --enable-tools \
  --enable-install-blobs \
  --enable-relocatable \
  --enable-strip \
  --disable-linux-user \
  --disable-bsd-user \
  --disable-plugins \
  --disable-guest-agent \
  --disable-docs \
  --disable-containers \
  --disable-debug-info \
  --disable-qom-cast-debug \
  --disable-werror \
  --prefix="${prefix}" \
  --bindir=bin \
  --libexecdir=libexec \
  --datadir=share \
  --with-suffix=qemu \
  "${host_args[@]}"
ninja -j 4 install

mkdir -p qemu/share/doc/qemu
install -m 0644 ../COPYING* ../LICENSE* qemu/share/doc/qemu/
case "${RUNNER_OS}" in
  Linux)
    for binary in qemu/bin/* qemu/libexec/*; do
      headers=$(LC_ALL=C readelf -lWd "${binary}")
      if grep -qE '^[[:space:]]+INTERP[[:space:]]|\(NEEDED\)' <<<"${headers}"; then
        echo "${binary} is dynamically linked" >&2
        exit 1
      fi
    done
    qemu/bin/qemu-system-"$(uname -m)" --version
    qemu/bin/qemu-img --version
    ;;
  macOS)
    # Include the notices from the libraries linked into the executables.
    docs=qemu/share/doc/qemu
    slirp_prefix=$(brew --prefix libslirp)
    install -m 0644 "${slirp_prefix}/COPYRIGHT" "${docs}/libslirp-COPYRIGHT"
    install -m 0644 "${slirp_prefix}/LICENSE" "${docs}/libslirp-LICENSE"
    install -m 0644 "$(brew --prefix pixman)/COPYING" "${docs}/pixman-COPYING"
    install -m 0644 "$(brew --prefix pcre2)/LICENCE.md" "${docs}/pcre2-LICENCE.md"
    for binary in qemu/bin/* qemu/libexec/*; do
      [[ -f "${binary}" ]] || continue
      non_system=$(otool -L "${binary}" | awk '/^[[:space:]]/ && $1 !~ /^\/(usr\/lib|System\/Library)\//')
      if [[ -n "${non_system}" ]]; then
        printf '%s links to non-system libraries:\n%s\n' "${binary}" "${non_system}" >&2
        exit 1
      fi
    done
    qemu/bin/qemu-system-aarch64 --version
    qemu/bin/qemu-img --version
    ;;
  Windows)
    system32=$(cygpath -S)
    for binary in qemu/bin/*.exe; do
      imports=$(LC_ALL=C objdump -p "${binary}")
      dlls=$(awk '/DLL Name:/ {print $NF}' <<<"${imports}")
      test -n "${dlls}"
      while IFS= read -r dll; do
        case "${dll,,}" in
          api-ms-win-* | ext-ms-win-*) continue ;;
        esac
        if [[ ! -f "${system32}/${dll}" ]]; then
          echo "${binary} imports non-system DLL ${dll}" >&2
          exit 1
        fi
      done <<<"${dlls}"
    done
    # Exercise the native loader with the MSYS2 and MinGW directories off PATH.
    PATH="${system32}" qemu/bin/qemu-system-x86_64.exe --version
    PATH="${system32}" qemu/bin/qemu-img.exe --version
    ;;
esac
if type -P gtar >/dev/null; then
    tar() { gtar "$@"; }
fi
tar -I 'zstd --ultra -22 -T0' -cf "../${ARCHIVE}" qemu
