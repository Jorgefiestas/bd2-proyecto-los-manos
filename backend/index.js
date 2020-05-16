const execSync = require('child_process').execSync
var express = require('express')
var cors = require('cors')
var bodyParser = require('body-parser')
var app = express()

app.use(cors())
app.use(bodyParser.json())

const insertToDB = (strat, register) => {
  console.log(Object.values(register).join(' '))
  execSync(`../webscripts/insert ${strat} ${Object.values(register).join(' ')}`)
  return true
}

const readFromDB = () => {
  return [
    [73982623, 'Arturo', 'Cuya', 20, '22/10/99'],
    [9283948, 'Mario', 'Pierini', 40, '12/12/12']
  ]
}

const searchInDB = (index, from, to = '') => {
  if (to.length > 0) {
    const output = execSync(`../webscripts/range_search ${index} ${from} ${to}`)
    console.log("range search output")
    console.log(output.toString())
  } else {
    console.log("search output")
    const output = execSync(`../webscripts/search ${index} ${from}`)
    console.log(output.toString())
  }

  return [[73982623, 'Arturo', 'Cuya', 20, '22/10/99']]
}

app.get('/read', (req, res) => {
  console.log('GET read')
  res.send(readFromDB())
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
