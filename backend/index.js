var express = require('express')
var cors = require('cors')
var app = express()

app.use(cors())

const insertToDB = (register) => {
  return true
}

const readFromDB = () => {
  return [
    [73982623, 'Arturo', 'Cuya', 20, '22/10/99'],
    [9283948, 'Mario', 'Pierini', 40, '12/12/12']
  ]
}

const searchInDB = () => {
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
  res.send(searchInDB())
})

app.listen(8080, () => console.log('Heider te quiero <3'))
