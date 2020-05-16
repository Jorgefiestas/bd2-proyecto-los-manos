const execSync = require('child_process').execSync
var express = require('express')
var cors = require('cors')
var bodyParser = require('body-parser')
var app = express()

app.use(cors())
app.use(bodyParser.json())

const insertToDB = (strat, register) => {
  execSync(`cd ../build; ./insert ${strat} ${Object.values(register).join(' ')}`)
  return true
}

const readFromDB = (strat) => {
  const output = execSync(`cd ../build; ./read ${strat}`)
  if (output.length > 0) {
  return output.toString().split('\n').map(row => row.split(' ')) 
  } else {
    return []
  }
}

const searchInDB = (index, from, to = '') => {
  if (to.length > 0) {
    const output = execSync(`cd ../build; ./range_search ${index} ${from} ${to}`)
    return output.toString().split('\n').map(row => row.split(' '))
  } else {
    const output = execSync(`cd ../build; ./search ${index} ${from}`)
    return [output.toString().split(' ')]
  }
}

app.post('/read', (req, res) => {
  console.log('GET read')
  res.send(readFromDB(req.body.strat))
})

app.post('/insert', (req, res)=> {
  const {strat, dni, nombres, apellidos, edad, fecha} = req.body
  const register = {dni,nombres,apellidos,edad,fecha}
  console.log('register')
  console.log(register)
  insertToDB(strat,register)

  res.send('Ok')
})

app.post('/search', (req, res)=> {
  console.log('POST search')
  const {from, to, strat} = req.body
  res.send(searchInDB(strat, from, to))
})

app.listen(8080, () => console.log('Heider te quiero <3'))
