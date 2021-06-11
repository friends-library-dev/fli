# fli

> A sorta replacement for `lerna link`, for the FLP multi-repo, written in C.

## Usage

_These assume you have the
[recommended aliases](https://github.com/friends-library-dev/manager/blob/master/bash_aliases.sh)
installed._

```bash
$ fli # symlink all the packages
$ flu # unlink all the packages
$ fli . # link just for cwd package
$ flu . # unlink just for cwd package
```

### TODO

- [ ] copy dependency files into `apps/native`, b/c react-native can't handle symlinks
