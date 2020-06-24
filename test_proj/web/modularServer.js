// file: modularServer.js

// modulos
var http = require('http')
var fs = require('fs');

// porta
const port = 3000


//variaveis com valores padrao pra pode starta
var totalAbertura = '7:00:00'; //abre totalmente
var metadeAbertura = '1:44:00'; //meio abertas
var fechamentoTotal = '20:00:00'; //fecha totalmente
var closeTimeOut = '00:01';

//função para comparar a hora e abrir ou fechar as cortinas na hora que o usuario configurou
const janelasAbreFecha = setInterval(() => {
  
  // cria o objeto hora
	const hora = new Date();

  //salva hora + min + seg
  const horaAgora = hora.getHours() + ':' +  hora.getMinutes() + ':' + hora.getSeconds();

  //console.log(horaAgora);
  //console.log(metadeAbertura); //hora mostrada no terminal
  //console.log(totalAbertura); //hora mostrada no terminal
  //console.log(fechamentoTotal); //hora mostrada no terminal

  if(horaAgora == metadeAbertura) {
    //manda o arduino deixar meio aberto
    var msg1 = ':' + slaveAdr + '07060512';
    var msg2 = ':' + slaveAdr + '07070512';
    sPort.write(msg1);
    console.log(msg1);
		
	} else if(horaAgora == totalAbertura) {
    //manda o arduino abrir completamente as janela
    var msg1 = ':' + slaveAdr + '07061024';
    var msg2 = ':' + slaveAdr + '07071024';
    sPort.write(msg1);
    console.log(msg1);

	} else if(horaAgora == fechamentoTotal) {
    //manda o arduino fecha as janela
    var msg1 = ':' + slaveAdr + '07060000';
    var msg2 = ':' + slaveAdr + '07070000';
    sPort.write(msg1);
    console.log(msg1);
  }
  
}, 1000); //faz isso a cada segundo


// servidor ouvindo em 'port'
var app = http.createServer(function (req, res) {

  var content = '';   // conteúdo da resposta
  var type = '';      // tipo da resposta: text/html, text/css

  if (req.url === '/') {   // requisição na raiz, carrega página principal
    content = fs.readFileSync("index.html");
    type = 'text/html';
  } else if (req.url === '/actions') {
    content = fs.readFileSync("actions.html");
    type = 'text/html';
  } else if (req.url === '/config') {
    content = fs.readFileSync("config.html");
    type = 'text/html';
  } else if (req.url === '/monitor') {
    content = fs.readFileSync("monitor.html");
    type = 'text/html';
  } else if (req.url === '/main.css') {
    content = fs.readFileSync("main.css");
    type = 'text/css';
  }

  console.log(req.url) // tipo de requisição realizada
  res.writeHead(200, { 'Content-Type': type }); // responde com tipo
  res.end(content + '\n'); // envia conteúdo 
}).listen(port)

var socket = require('socket.io').listen(app);

socket.on('connection', function (client) {
  //aqui monta as mensagens e envia para o firmware
  client.on('state', function (data) {
    console.log('Valor de tempo recebido do HTML:' + data);
    slaveCmd = data[0]
    slaveOut = data[1]
    slaveState = data[2]
    mensagem = ':' + slaveAdr + slaveCmd + slaveOut + slaveState
    sPort.write(mensagem)
    console.log(mensagem)
  })

  //importando as variaveis da pag config para usar na função abreefecha
  client.on('Janelas', function (data) {
    console.log('Variaveis hora da pag config:' + data);
    metadeAbertura = data[0];
    totalAbertura = data[1];
    fechamentoTotal = data[2];
    console.log('Hora de total abertura = ' 
    + metadeAbertura + ', Meia abertura = ' 
    + totalAbertura + ', Fechamento total = ' 
    + fechamentoTotal + ' Estão no servidor');
})
  
  //salva o tempo do alarme pra disparar se a porta ficar aberta
  client.on('Alarme', function (data) {
    console.log('Variaveis hora da pag config:' + data);
    closeTimeOut = data;
    console.log('Tempo pra desparar o alarme = ' 
    + closeTimeOut + ', Está no servidor '); 
  })
})

/*****   Porta Serial *****/
const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
const sPort = new SerialPort('COM6', { baudRate: 9600 })
const parser = new Readline();

sPort.pipe(parser);

var slaveAdr = '03';
var slaveCmd = '05';
var slaveOut = '01';
var slaveState = '0000';
var mensagem = ':' + slaveAdr + slaveCmd + slaveOut + slaveState;

sPort.open(function (err) {
  if (err) {
    console.log(err.message)
  }
  console.log('Porta Serial Aberta')
})

//filtra a mensagem recebida do firmware e envia para atualiza página monitor
parser.on('data', (data) => {
  var new_data = data.split('');
  console.log(data);
  //console.log('new data    ' + new_data);
  if (new_data[21] == ':') {
    // OP = 1 É PORTA
    if ((new_data[26] == '0') && (new_data[27] == '2') && (new_data[28] == 'F') && (new_data[29] == 'F')) {
      socket.emit('retorno', [ 1, 'ABERTA']);
    }
    if ((new_data[26] == '0') && (new_data[27] == '2') && (new_data[28] == '0') && (new_data[29] == '0')) {
      socket.emit('retorno', [ 1, 'FECHADA']);
    }
    // OP = 2 É JANELA
    if ((new_data[26] == '0') && (new_data[27] == '3') && (new_data[28] == '0') && (new_data[29] == '0')) {
      socket.emit('retorno', [ 2, 'DESLIGADO']);
    }
    if ((new_data[26] == '0') && (new_data[27] == '3') && (new_data[28] == 'F') && (new_data[29] == 'F')) {
      socket.emit('retorno', [ 2, 'LIGADO']);
    }
    // OP = 3 É TEMPERATURA DO BANHEIRO
    if ((new_data[24] == '0') && (new_data[25] == '6') && (new_data[26] == '0') && (new_data[27] == '4')) {
      var dado = new_data[30]+new_data[31];
      socket.emit('retorno', [ 3, dado]);
    }
    // OP = 4 É VELOCIDADE DO AR
    if ((new_data[24] == '0') && (new_data[25] == '6') && (new_data[26] == '0') && (new_data[27] == '5')) {
      var dado = new_data[29]+new_data[30]+new_data[31];
      socket.emit('retorno', [ 4, dado]);
    }
    // OP = 5 É ALERTA VENTO FORTE
    if ((new_data[24] == '0') && (new_data[25] == '6') && (new_data[26] == '0') && (new_data[27] == '6') && (new_data[28] == '0') && (new_data[29] == '0')) {
      socket.emit('retorno', [ 5, 'DESLIGADO']);
    }
    if ((new_data[24] == '0') && (new_data[25] == '6') && (new_data[26] == '0') && (new_data[27] == '6') && (new_data[28] == 'F') && (new_data[29] == 'F')) {
      socket.emit('retorno', [ 5, 'LIGADO']);
    }
    // OP = 6 CORTINA SALA JANTAR
    if ((new_data[24] == '0') && (new_data[25] == '6') && (new_data[26] == '0') && (new_data[27] == '7')) {
      var dado = new_data[28]+new_data[29]+new_data[30]+new_data[31];
      socket.emit('retorno', [ 6, dado]);
    }

    // OP = 7 CORTINA SALA ESTAR
    if ((new_data[24] == '0') && (new_data[25] == '6') && (new_data[26] == '0') && (new_data[27] == '8')) {
      var dado = new_data[28]+new_data[29]+new_data[30]+new_data[31];
      socket.emit('retorno', [ 7, dado]);
    }
    // OP = 8 É TEMPERATURA DO AC
    if ((new_data[24] == '0') && (new_data[25] == '6') && (new_data[26] == '0') && (new_data[27] == '3')) {
      var dado = new_data[30]+new_data[31];
      socket.emit('retorno', [ 8, dado]);
    }
  }
})
