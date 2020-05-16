const execSync = require('child_process').execSync
var express = require('express')
var cors = require('cors')
var bodyParser = require('body-parser')
var app = express()

app.use(cors())
app.use(bodyParser.json())

const insertToDB = (register) => {
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
    console.log(output.toString())
  } else {
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
  console.log('POST insert')
  res.send('Ok')
})

app.post('/search', (req, res)=> {
  console.log('POST search')
  const {from, to, strat} = req.body
  res.send(searchInDB(strat, from, to))
})

app.listen(8080, () => console.log('Heider te quiero <3'))
