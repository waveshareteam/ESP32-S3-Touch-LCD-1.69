# Release Artifacts

GitHub Actions writes packaged firmware archives to `releases/dist/` during CI runs. The generated archives are ignored by Git and should be downloaded from workflow artifacts.

Factory firmware remains under `firmware/` because it is a released recovery artifact, not a CI-generated build output.
