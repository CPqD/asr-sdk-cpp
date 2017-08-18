CPqD ASR Recognizer
===================

O Recognizer é uma API para criação de aplicações de voz que utilizam o servidor CPqD ASR para reconhecimento de fala.


Para maiores informações, consulte [a documentação do projeto](https://speech-doc.cpqd.com.br/asr).

### Códigos de Exemplo

Códigos de exemplo estão sob o diretório `examples` do repositório

### Dependências

* cmake >= 2.8.12
* Biblioteca de desenvolvimento ALSA
* * Debian/Ubuntu e derivados: `apt-get install libasound2-dev`
* * RedHat/CentOS/Fedora e derivados: `yum install alsa-lib-devel`

### Compilando a partir do código-fonte

Baixe a última versão do repositório `git pull origin master` e execute:

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 -DBUILD_EXAMPLES=1
    make

Para mais opções de compilação, recomendamos o uso da ferramenta `cmake-gui`

Licença
-------

Copyright (c) 2017 CPqD. Todos os direitos reservados.

Publicado sob a licença Apache Software 2.0, veja LICENSE.
