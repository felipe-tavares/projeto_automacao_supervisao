// file: modularServer.js

// modulos
var http = require('http')
var fs = require('fs');

// porta
const port = 3000

/*const interval = setInterval(() => {
	// Pega o horário atual
	const now = new Date();

	// Formata a hora (hh:mm:ss)
	const dataHora = zeroFill(now.getHours()) + ':' + zeroFill(now.getMinutes()) + ':' + zeroFill(now.getSeconds());
	console.log(dataHora); //hora mostrada no terminal
	Hora = dataHora;

	if(Hora == windop){
		//envia comando ao firmware para abrir janelas
		
	}else if(Hora == windhalf){
		//envia comando ao firmware para deixar janelas meio abertas
		
	}else if(Hora == windclose){
		//envia comando ao firmware para fechar janelas
	}
}, 1000);//a cada segundo... */

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
  client.on('state', function (data) {
    console.log('Valor de tempo recebido do HTML:' + data);
    slaveCmd = data[0]
    slaveOut = data[1]
    slaveState = data[2]
    mensagem = ':' + slaveAdr + slaveCmd + slaveOut + slaveState
    sPort.write(mensagem)
    console.log(mensagem)
  })
})

/*****   Porta Serial *****/
const SerialPort = require('serialport');
const Readline = SerialPort.parsers.Readline;
const sPort = new SerialPort('COM5', { baudRate: 9600 })
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

parser.on('data', (data) => {
  var new_data = data.split('');
  console.log(data);
  if (new_data[21] == ':') {
    if ((new_data[24] == '0') && (new_data[25] == '1')) {
      socket.emit('retorno', define_estado(new_data));
    }
    if ((new_data[24] == '0') && (new_data[25] == '2')) {
      socket.emit('retorno', define_estado(new_data));
    }
    if ((new_data[24] == '0') && (new_data[25] == '6')) {
      socket.emit('retorno', [new_data[27], new_data[25], [new_data[28], new_data[29], new_data[30], new_data[31]]]);
    }
  }
})

function define_estado(data) {
  if ((data[28] && data[29]) == 'F') {
    return [data[27], data[25], 'Ligado'];
  }
  if ((data[28] && data[29]) == '0') {
    return [data[27], data[25], 'Desligado'];
  }
}

/*atualizaMonitor(function() {
  flag = flag + incremento;
  console.log('Enviando valor de flag: ' + flag);
  console.log('Valor atual do incremento: ' + incremento);
  socket.emit('atualizaFlag', flag);
  }, 1000)*/