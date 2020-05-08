## atropine 

Part of the [n1ghtshade](https://github.com/synackuk/n1ghtshade) jailbreak.

### Produces two targets

* hooker - an iBoot payload designed to facilitate the loading of the payload into the bootchain.
* payload - Designed to patch an image (iBEC/iBoot/Kernel) and hook itself into the execution of subsequent images - for instance if an iBoot executes a kernelcache atropine hooks the execution of the kernel to first patch it.