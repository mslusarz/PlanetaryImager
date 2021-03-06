import shutil
from dockerfile import Dockerfile


class LinuxBase(Dockerfile):
    def __init__(self, snippet, flavour, version, arch, extra_snippets = [], extra_substitutions={}):
        self.flavour = flavour
        self.version = version
        self.arch = arch
        name = '{}-{}-{}'.format(flavour, version, arch)
        base_image = '{}:{}'.format(flavour, version)
        config_name = '{}-{}'.format(flavour, version)
        self.cmake_binary = 'cmake'
        self.os_family = 'linux'

        if arch != 'x86_64':
            base_image = arch + '/' + base_image

        configuration_file = 'configuration-{}.cmake'.format(config_name)

        substitutions = {
            'CONFIG_NAME': config_name,
            'BASE_IMAGE': base_image,
            'CMAKE_BIN': self.cmake_binary,
            'CMAKE_CACHE_INIT': configuration_file,
            'PACKAGE_SYSTEM_NAME': '{}-{}'.format(flavour, version),
        }
        substitutions.update(extra_substitutions)

        snippets = [snippet]
        snippets.extend(extra_snippets)

        files = []

        self.qemu_arm_static = shutil.which('qemu-arm-static')
        if self.qemu_arm_static is not None:
            files.append(self.qemu_arm_static)

        super().__init__(name, snippets, files, substitutions)

    def build(self, stderr=False):
        if self.qemu_arm_static is None:
            raise RuntimeError('qemu-arm-static not found in PATH')
        super().build(stderr)

 

