**Projeto IoT - G1: Sistema de Iluminação Automatizada com ESP32, MQTT e Cloudflare**
**Dupla: Hérica Becker e Arthur Zanon**

**Objetivos:**
Desenvolver um sistema de automação que acione uma luz com base na detecção de movimento (sensor PIR) e baixa luminosidade ambiente (sensor LDR).
Implementar controle e monitoramento remoto em tempo real utilizando o protocolo MQTT.
Armazenar dados dos sensores em um banco de dados na nuvem (Cloudflare D1) através do Cloudflare Worker.

**1. Componentes e Tecnologias Utilizadas:**
Hardware:
Microcontrolador: ESP32;
Sensor de Movimento: PIR (Sensor Infravermelho Passivo), para detectar movimento;
Sensor de Luminosidade: LDR (Resistor Dependente de Luz), para medir a intensidade da luz ambiente;
Atuador: LED, para ligar quando ouver movimento e a luz estiver apagada, ligar;
Software e Protocolos:
Ambiente de Desenvolvimento: Framework Arduino com a IDE do Visual Code (PlatformIO);
Linguagem: C++;
Protocolo de Mensagem: MQTT (Message Queuing Telemetry Transport), para comunicação em tempo real entre o ESP32;
Protocolo de Comunicação: HTTP/JSON, utilizado para enviar os dados coletados para a API do Cloudflare Worker;
Plataforma de Cloud:
Broker MQTT: test.mosquitto.org;
Cloudflare Workers: Plataforma utilizada para criar uma API que recebe os dados do ESP32 via HTTP POST;
Cloudflare D1: Banco de dados SQL onde os dados dos sensores são armazenados;

**2. Arquitetura da Solução:**
A solução é dividida em três camadas principais:
Camada de Dispositivo (ESP32) responsável por:
Ler os dados dos sensores LDR e PIR.
Aplicar a lógica de controle para acionar o LED.
Conectar-se ao Wi-Fi.
Comunicar-se em duas vias distintas:
Comunicação em Tempo Real (MQTT): Publica o estado atual dos sensores e do LED em tópicos MQTT específicos e se inscreve em um tópico de comando para receber ordens remotas (Ligar, Desligar, Mudar para modo automático).
Persistência de Dados (HTTP): Envia periodicamente um pacote de dados em formato JSON para um endpoint HTTP (Cloudflare Worker).
Camada de Comunicação e Lógica (Cloud):
O Broker MQTT atua como um intermediário, distribuindo as mensagens publicadas pelo ESP32 para qualquer cliente inscrito (como um painel de controle ou aplicativo) e vice-versa.
O Cloudflare Worker atua como uma API REST. Ele recebe a requisição HTTP POST do ESP32, processa o JSON recebido e executa o comando SQL para inserir os dados no banco de dados.
Camada de Armazenamento e Visualização (Cloud):
O Cloudflare D1 armazena os dados históricos enviados pelo dispositivo.
Esses dados podem ser posteriormente consumidos por painéis de visualização (dashboards como Grafana), aplicativos móveis ou ferramentas de análise para entender padrões de uso.

**3. Análise do Código:**
O firmware do ESP32 foi estruturado para ser modular e resiliente, garantindo a reconexão automática e a separação de responsabilidades.
Principais Funcionalidades:
Modos de Operação:
Modo Automático (manualMode = false): É o comportamento padrão. O LED é acionado somente se a leitura do LDR estiver abaixo de um limiar (LDR_THRESHOLD) e o sensor PIR detectar movimento. Esta lógica visa a máxima eficiência energética.
Modo Manual (manualMode = true): O controle automático é desabilitado. O estado do LED passa a ser controlado exclusivamente por comandos recebidos via MQTT no tópico esp32/esp32_sala/led/command. Os comandos aceitos são:
ON: Liga o LED.
OFF: Desliga o LED.
AUTO: Retorna o sistema para o modo automático.
Comunicação MQTT:
Publicação de Estado: A cada 5 segundos (intervalMqttPub), o dispositivo publica um JSON completo no tópico esp32/esp32_sala/state, contendo a leitura do LDR, o estado do sensor de movimento, o estado do LED e o modo de operação atual.
Tópicos Dedicados: O estado do LED (ON/OFF) também é publicado em um tópico separado (esp32/esp32_sala/led/state) para facilitar integrações com interfaces simples.
Last Will and Testament (LWT): O dispositivo está configurado para publicar a mensagem offline no tópico esp32/esp32_sala/status caso a conexão com o broker seja perdida inesperadamente, permitindo que sistemas de monitoramento saibam que o dispositivo não está mais online.
Integração com Banco de Dados via Cloudflare Worker:
A cada 30 segundos (intervalDbSend), a função sendDataToCloudflare() é acionada.
Esta função monta um objeto JSON com os dados: device_id, ldr, movimento e led.
Uma requisição HTTP POST é enviada para a URL do Worker (https://projeto-g1.zanondea.workers.dev/insert).
O Worker, por sua vez, recebe esses dados e os insere no banco de dados Cloudflare D1, garantindo um registro histórico do comportamento do ambiente.


![**4. Fluxograma da Lógica Operacional**](https://github.com/ZanonDeAndrade/IoT/blob/main/Sem%20t%C3%ADtulo.jpeg)



**5. Conclusão**
O projeto demonstrou com sucesso a criação de um sistema de IoT funcional e completo, integrando hardware de baixo custo com serviços de nuvem modernos e eficientes.
A utilização do protocolo MQTT garante o controle e monitoramento em tempo real com baixa latência, enquanto a integração com Cloudflare Workers e D1 proporciona uma maneira de coletar e armazenar dados para análise de padrões.

