import os
import sys
import subprocess

'''
current_wd = os.getcwd()
back_one_dir = os.path.join(os.path.dirname(__file__), "..")
os.chdir(back_one_dir)
shared_libs_folder = os.path.join(os.getcwd(), "shared")
os.chdir(current_wd)
'''


class Dependency(object):
    def __init__(self, dir, external_dir="external"):
        self.dir = dir
        self.external_dir = external_dir

    def compile(self):
        pass

    def run_shell_command(self, command):
        result = subprocess.run(
            command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        print(result.stdout)
        return result


class CmakeGithubDependency(Dependency):
    def __init__(self, dir, url, tag, cmake_build_args="", cmake_install_args=""):
        super().__init__(dir)
        self.url = url
        self.tag = tag
        self.git_repo_path = os.path.join(os.path.dirname(__file__),
                                          self.external_dir, self.dir)
        self.cmake_build_args = cmake_build_args
        self.cmake_install_args = cmake_install_args
        self.flag_possui_submodulos = None
        self.build_dir = "build"
        self.submodules = []
        # self.shared_libs_folder = shared_libs_folder

    def clone_repo(self):
        print(f"Clonando repositório {self.url}")
        current_dir = os.path.dirname(__file__)
        git_clone_path = os.path.join(os.path.dirname(__file__),
                                      self.external_dir)

        os.chdir(git_clone_path)
        cmake_run = self.run_shell_command(
            f"git clone {self.url} -b {self.tag} {self.dir}")

        if self.possui_submodulos():
            git_pull_modules = self.run_shell_command(
                'git submodule update --init --recursive')

            if git_pull_modules.returncode == 0:
                print(f"{self.url} submodules initialized")

        os.chdir(current_dir)

    def possui_submodulos(self):
        if self.flag_possui_submodulos != None:
            return self.flag_possui_submodulos

        if os.path.exists(os.path.join(self.git_repo_path, '.gitmodules')):
            git_get_modules_run = self.run_shell_command(
                'git config -f .gitmodules -l')

            if git_get_modules_run.returncode == 0:
                self.flag_possui_submodulos = True

                modules_found = {}
                for line in git_get_modules_run.stdout.split("\n"):
                    if line:
                        splitted_line = line.split(".", 2)
                        # [0] submodules
                        # [1] submodule name
                        # [2] <attr>=<value>
                        splitted_attr = splitted_line[2].split("=", 1)
                        attr = splitted_attr[0]
                        value = splitted_attr[1]
                        if splitted_line[1] not in modules_found:
                            modules_found[splitted_line[1]] = {}
                        modules_found[splitted_line[1]][attr] = value

                for module in modules_found:
                    print(modules_found[module])
                    self.submodules.append(modules_found[module])

        print(self.submodules)
        self.flag_possui_submodulos = False
        return self.flag_possui_submodulos

    def compile_submodule(self, path, url, generator='"MinGW Makefiles"'):
        print(
            f"compilando submódulo {url}")
        submodule_dir = os.path.join(
            self.git_repo_path, path)
        os.chdir(submodule_dir)

        submodule_build_dir = os.path.join(
            submodule_dir, self.build_dir)
        if not os.path.exists(submodule_build_dir):
            os.mkdir(self.build_dir)

        os.chdir(submodule_build_dir)

        cmake_run = self.run_shell_command(f'cmake .. -G {generator}')

        cmake_build = self.run_shell_command(
            f'cmake --build {submodule_build_dir}')

        cmake_install = self.run_shell_command(
            f'cmake --install {submodule_build_dir} {self.cmake_install_args}')

        os.chdir(self.git_repo_path)

    def update_repo(self):
        print(f"Realizando pull do repositório {self.url}")
        current_dir = os.path.dirname(__file__)

        os.chdir(self.git_repo_path)
        cmake_run = self.run_shell_command(
            f"git pull")

        if self.possui_submodulos():
            print(f"Realizando pull dos submodulos para {self.url}")
            git_pull_modules = self.run_shell_command(
                'git pull --recurse-submodules')

            if git_pull_modules.returncode == 0:
                print(f"{self.url} submodules pulled")

        os.chdir(current_dir)

    def compile(self, generator='"MinGW Makefiles"'):
        print(f"compiling {self.url} repository")
        need_to_clone = False
        print('Checking if repository is initialized:')
        if os.path.exists(self.git_repo_path):
            os.chdir(self.git_repo_path)

            git_check = self.run_shell_command(
                "git rev-parse --is-inside-work-tree")
            if git_check.returncode != 0:
                need_to_clone = True
        else:
            print('False')
            need_to_clone = True

        if need_to_clone:
            self.clone_repo()
        else:
            self.update_repo()

        for submodule in self.submodules:
            self.compile_submodule(submodule['path'], submodule['url'])

        current_dir = os.path.dirname(__file__)

        dependency_build_dir = os.path.join(self.git_repo_path, self.build_dir)
        if not os.path.exists(dependency_build_dir):
            os.mkdir(self.build_dir)

        os.chdir(dependency_build_dir)

        cmake_run = self.run_shell_command(f'cmake .. -G {generator}')
        print("cmake Executado. Iniciando processo de build e install")

        cmake_build = self.run_shell_command(
            f'cmake --build {dependency_build_dir} {self.cmake_build_args}')

        cmake_install = self.run_shell_command(
            f'cmake --install {dependency_build_dir} {self.cmake_install_args}')

        os.chdir(current_dir)


def program():
    current_dir = os.path.dirname(__file__)
    dependencies = []
    dependencies.append(CmakeGithubDependency(
        dir="sdl2", url="https://github.com/libsdl-org/SDL.git", tag="SDL2", cmake_install_args=f"--prefix {current_dir}"))
    dependencies.append(CmakeGithubDependency(
        dir="sdl2-image", url="https://github.com/libsdl-org/SDL_image.git", tag="SDL2", cmake_install_args=f"--prefix {current_dir}"))

    for dependency in dependencies:
        dependency.compile()


if __name__ == '__main__':
    with open('compiles-output-log.txt', 'w') as sys.stdout:
        print('Output do programa está em compiles-output-log.txt')

        if os.name != "nt":
            program()
        else:
            import ctypes
            import sys

            def is_admin():
                try:
                    return ctypes.windll.shell32.IsUserAnAdmin()
                except:
                    return False

            if is_admin():
                program()
            else:
                # Re-run the program with admin rights
                ctypes.windll.shell32.ShellExecuteW(
                    None, "runas", sys.executable, " ".join(sys.argv), None, 1)

'''
def get_produtos_a_venda():
    return [Produto(preco=5, quantidade_em_estoque=100, valor_de_fabrica=4),
            Produto(preco=5, quantidade_em_estoque=100, valor_de_fabrica=4),
            Cafe(preco=2, quantidade_em_estoque=50, valor_de_fabrica=0)]


class Produto:
    def __init__(self, preco, quantidade_em_estoque, valor_de_fabrica):
        self.preco = preco
        self.valor_de_fabrica = valor_de_fabrica
        self.quantidade_em_estoque = quantidade_em_estoque

    def venda(self, produtos_vendidos=1):
        self.quantidade_em_estoque -= produtos_vendidos
        print(f"produto vendido por {self.preco} reais")

    def promocao(self, reducao_de_preco):
        if self.preco <= reducao_de_preco:
            print("não pode fazer essa promocao, senao vou ter prejuizo")
        else:
            self.preco -= reducao_de_preco


class Cafe(Produto):
    def __init__(self, preco, quantidade_em_estoque, valor_de_fabrica=0):
        super().__init__(preco, valor_de_fabrica, quantidade_em_estoque)

    def promocao(self, reducao_de_preco):
        raise Exception("Produto não pode entrar em promoção")


class Livro(Produto):
    pass


produtos = get_produtos_a_venda()

for produto in produtos:
    try:
        produto.venda()
        produto.promocao()
    except Exception as e:
        print(e)
'''
