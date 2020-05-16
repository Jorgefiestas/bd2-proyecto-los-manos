import Head from 'next/head'
import axios from 'axios'

import { useState, useEffect } from 'react'

import {
  Grid,
  TextField,
  MenuItem,
  Button,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Paper
} from '@material-ui/core'
import Tabs from '@material-ui/core/Tabs'
import Tab from '@material-ui/core/Tab'
import Text from '@material-ui/core/Typography'

const Index = () => {
  const initSearch = {
    from: '',
    to: ''
  }

  const initInsert = {
    dni: '',
    nombres: '',
    apellidos: '',
    edad: '',
    fecha: ''
  }

  const url =
    process.env.NODE_ENV === 'development'
      ? 'http://localhost:8080'
      : 'http://localhost:8080'
  const [state, setState] = useState({
    tab: 0,
    strat: 0,
    table: [],
    search: initSearch,
    insert: initInsert,
    operation: ''
  })

  const read = async () => {
    const result = await axios.get(`${url}/read`)

    setState({
      ...state,
      table: result.data,
      search: initSearch,
      insert: initInsert
    })
  }

  const search = async () => {
    const result = await axios.post(`${url}/search`, {
      ...state.search,
      strat: state.strat
    })

    console.log(result.data)

    setState({
      ...state,
      operation: `Búsqueda ${
        state.strat === 2 && state.search.to.length > 0
          ? 'por rango'
          : ''
      }`,
      table: result.data
    })
  }

  const insert = async () => {
    const result = await axios.post(`${url}/search`, state.search)
    setState({
      ...state,
      operation: 'Inserción de nuevo registro'
    })
    read()
  }

  useEffect(() => {
    read()
  }, [])

  return (
    <>
      <Head>
        <title>Proyecto 1 | BD2</title>
      </Head>
      <Grid container justify="center">
        <Grid container direction="column" style={{ width: '70vw' }}>
          <div></div>
          <Text variant="h2">Proyecto BD 2</Text>
          <Text>Integrantes</Text>
          <ul>
            <li>
              <Text>Jorge Fiestas</Text>
            </li>
            <li>
              <Text>Martin Carrasco</Text>
            </li>
            <li>
              <Text>Arturo Cuya</Text>
            </li>
          </ul>
          <TextField
            select
            label="Estrategia de indexación"
            helperText="Solo B+ Tree permite búsqueda por rango"
            variant="outlined"
            value={state.strat}
            onChange={e =>
              setState({ ...state, strat: e.target.value })
            }
            style={{ width: 300, marginTop: 20 }}
          >
            <MenuItem value={0}>Dynamic Hash</MenuItem>
            <MenuItem value={1}>Random Page</MenuItem>
            <MenuItem value={2}>B+ Tree</MenuItem>
          </TextField>
          <Tabs
            value={state.tab}
            onChange={(e, v) => setState({ ...state, tab: v })}
          >
            <Tab label="Búsqueda" />
            <Tab label="Inserción" />
          </Tabs>
          <Grid
            container
            style={{ padding: 10, backgroundColor: '#fcfcfc' }}
          >
            {state.tab == 0 ? (
              <>
                <Grid
                  container
                  alignItems="center"
                  style={{ width: 400 }}
                >
                  <TextField
                    variant="outlined"
                    label="Ingresar DNI"
                    value={state.search.from}
                    onChange={e =>
                      setState({
                        ...state,
                        search: {
                          ...state.search,
                          from: e.target.value
                        }
                      })
                    }
                  />
                  <Button
                    variant="contained"
                    style={{
                      backgroundColor: 'royalblue',
                      color: '#fff',
                      marginLeft: 20
                    }}
                    onClick={search}
                  >
                    Buscar
                  </Button>
                </Grid>
                {state.strat === 2 && (
                  <Grid
                    container
                    direction="column"
                    style={{ width: 400 }}
                  >
                    <Text>Búsqueda por rango</Text>
                    <TextField
                      variant="outlined"
                      label="DNI desde"
                      style={{ marginBottom: 20 }}
                      value={state.search.from}
                      onChange={e =>
                        setState({
                          ...state,
                          search: {
                            ...state.search,
                            from: e.target.value
                          }
                        })
                      }
                    />
                    <TextField
                      variant="outlined"
                      label="DNI hasta"
                      style={{ marginBottom: 20 }}
                      value={state.search.to}
                      onChange={e =>
                        setState({
                          ...state,
                          search: {
                            ...state.search,
                            to: e.target.value
                          }
                        })
                      }
                    />
                    <Button
                      variant="contained"
                      style={{
                        backgroundColor: 'royalblue',
                        color: '#fff',
                        width: 200
                      }}
                      onClick={search}
                    >
                      Buscar por rango
                    </Button>
                  </Grid>
                )}
              </>
            ) : (
              <Grid
                container
                direction="column"
                style={{ width: 400 }}
              >
                <TextField
                  className="field"
                  label="DNI"
                  variant="outlined"
                  style={{ marginBottom: 20 }}
                  value={state.insert.dni}
                  onChange={e =>
                    setState({
                      ...state,
                      insert: { ...state.insert, dni: e.target.value }
                    })
                  }
                />
                <TextField
                  variant="outlined"
                  className="field"
                  label="Nombres"
                  style={{ marginBottom: 20 }}
                  value={state.insert.nombres}
                  onChange={e =>
                    setState({
                      ...state,
                      insert: {
                        ...state.insert,
                        nombres: e.target.value
                      }
                    })
                  }
                />
                <TextField
                  variant="outlined"
                  className="field"
                  label="Apellidos"
                  style={{ marginBottom: 20 }}
                  value={state.insert.apellidos}
                  onChange={e =>
                    setState({
                      ...state,
                      insert: {
                        ...state.insert,
                        apellidos: e.target.value
                      }
                    })
                  }
                />
                <TextField
                  variant="outlined"
                  className="field"
                  label="Edad"
                  style={{ marginBottom: 20 }}
                  value={state.insert.edad}
                  onChange={e =>
                    setState({
                      ...state,
                      insert: {
                        ...state.insert,
                        edad: e.target.value
                      }
                    })
                  }
                />
                <TextField
                  variant="outlined"
                  className="field"
                  label="Fecha de nacimiento"
                  helperText="Formato: dd/mm/aaaa"
                  style={{ marginBottom: 20 }}
                  value={state.insert.fecha}
                  onChange={e =>
                    setState({
                      ...state,
                      insert: {
                        ...state.insert,
                        fecha: e.target.value
                      }
                    })
                  }
                />
                <Button
                  variant="contained"
                  style={{
                    backgroundColor: 'royalblue',
                    color: '#fff'
                  }}
                  onClick={insert}
                >
                  Insertar
                </Button>
              </Grid>
            )}
          </Grid>
          <Text variant="h4">Tabla de Resultados</Text>
          {state.operation.length > 0 && (
            <Text>Operación realizada: {state.operation}</Text>
          )}
          <TableContainer component={Paper}>
            <Table>
              {state.table.length == 0 && (
                <caption>No se encontraron resultados</caption>
              )}
              <TableHead>
                <TableRow>
                  <TableCell>DNI</TableCell>
                  <TableCell>Nombres</TableCell>
                  <TableCell>Apellidos</TableCell>
                  <TableCell>Edad</TableCell>
                  <TableCell>Fecha de nac.</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {state.table.map((row, i) => (
                  <TableRow key={i}>
                    {row.map((col, j) => (
                      <TableCell key={j}>{col}</TableCell>
                    ))}
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </TableContainer>
        </Grid>
      </Grid>
    </>
  )
}

export default Index
