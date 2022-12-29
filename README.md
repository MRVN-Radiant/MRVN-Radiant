# MRVNRadiant

The open-source, cross-platform level editor for Respawn Entertainment Source based games.

MRVNRadiant is a fork of NetRadiant-custom (GtkRadiant 1.4 &rarr; massive rewrite &rarr; 1.5 &rarr; NetRadiant &rarr; NetRadiant-custom &rarr; this)

<div align=left>
<img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/MRVNRadiant/MRVNRadiant/build.yml?style=for-the-badge">
<img alt="GitHub Issues" src="https://img.shields.io/github/issues/MRVNRadiant/MRVNRadiant?style=for-the-badge">
<img alt="GitHub Pullrequests" src="https://img.shields.io/github/issues-pr/MRVNRadiant/MRVNRadiant?style=for-the-badge">
</div>

## Supported games
- Titanfall Online ( Titanfall Online Revival )
- Titanfall 2 ( [Northstar](https://northstar.tf) )
- Apex Legends ( R5Reloaded )

## Status
| Game | Coverage | Note |
|------|----------|------|
| Titanfall Online | 38% | Basic collision, no lighting, needs testing |
| Titanfall 2 | 37% | Basic collision, no lighting, meshes flicker |
| Apex Legends | 25% | No collision, no lighting |

> Coverage = Lumps generated / Total lump count. Stubbed lumps arent counted because they're not custom.

> NOTE: These values are updated manually.

## Other tools
- [bsp_tool](https://github.com/snake-biscuits/bsp_tool)
    - A Python library for analyzing .bsp files
- [rbsp-explorer](https://gitlab.com/F1FTY/rbsp-explorer)
    - OpenGL / ImGui respawn bsp explorer
