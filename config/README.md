## Config
This dir contains example of kernel and buildroot configuration.
They have so many configs and it is hard to decide which should be which for the first.
These configs are relatively used generally and not heavy to build.

## Contents
- [.config](.config)
  - Config file for buildroot. Place it on the root dir of buildroot. This is from buildroot.2020.11.2.
- [.config.linux](.config.linux)
  - Config file for linux. Rename it to `.config` and place it on the `<buildroot dir>/output/buildr/linux-<ver>/`. This is from Linux v5.9.18.
