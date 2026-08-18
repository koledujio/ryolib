# D3D11 graphics backend
by koledujio

Calls for each renderer *need* to happen on the same thread, that thread can be any secondary or main thread.

D3D11's context *is not* thread-safe.
Multiple threads *can* however be used for multiple renderers.

## To-Do:
- [x] Refactor into smaller directories
- [ ] Optimize sampler creation(create in _renderer_create, reuse instead of recreating)