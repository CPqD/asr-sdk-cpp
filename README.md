CPqD ASR Recognizer
===================

O Recognizer é uma API para criação de aplicações de voz que utilizam o servidor CPqD Reconhecimento de Fala (CPqD ASR).

Para maiores informações sobre o CPqD ASR, consulte o [site do produto](http://speechweb.cpqd.com.br/asr/docs/latest/).

Para entender como usar esta biblioteca, sugerimos verificar e executar os exemplos, em particular [basic.cc](https://github.com/CPqD/asr-sdk-cpp/blob/master/examples/basic.cc).

Os códigos de exemplo estão sob o diretório `examples` do repositório

### Dependências

* cmake >= 2.8.12
* Biblioteca de desenvolvimento ALSA
  * Debian/Ubuntu e derivados: `apt-get install libasound2-dev`
  * RedHat/CentOS/Fedora e derivados: `yum install alsa-lib-devel`

### Compilando a partir do código-fonte

Baixe a última versão do repositório `git pull origin master` e execute:

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1 -DBUILD_EXAMPLES=1
    make

Para mais opções de compilação, recomendamos o uso da ferramenta `cmake-gui`

### Executando um teste

Depois de compilar, a partir do diretório `build/examples` criado, execute um teste usando o exemplo `basic.cc`:

    export LD_LIBRARY_PATH=$PWD/src

    ./basic ws://127.0.0.1:8025/asr-server/asr builtin:grammar/samples/phone ../../examples/audio/phone-1937050211-8k.wav

**Você deve mudar o IP do servidor ASR de 127.0.0.1 para o IP correto do servidor ASR na sua instalação.**

Supomos que esteja usando modelos para áudio de 8kHz, caso contrário, deve testar com o arquivo de áudio de 16kHz:

    export LD_LIBRARY_PATH=$PWD/src

    ./basic ws://127.0.0.1:8025/asr-server/asr builtin:grammar/samples/phone ../../examples/audio/phone-1937050211-16k.wav

### Usando a biblioteca em aplicações

A biblioteca cliente pode ser usada em sua aplicação através do arquivo
`libasr-client.so` criado em `build/src` e dos arquivos de *header* presentes em
`include/cpqd/asr-client`.

Licença
-------

Copyright (c) 2017 CPqD. Todos os direitos reservados.

Publicado sob a licença Apache Software 2.0, veja LICENSE.
