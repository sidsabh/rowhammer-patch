#!/usr/bin/env node
import { Instruction } from '../rvcodecjs/core/Instruction.js'

// --- helper functions ---
function parseEncoding(enc) {
  if (/^[01]+$/.test(enc)) {
    return parseInt(enc, 2) >>> 0
  } else {
    return parseInt(enc, 16) >>> 0
  }
}

function flipBit(intVal, i) {
  const flipped = (intVal ^ (1 << i)) >>> 0
  return flipped.toString(16).padStart(8, '0').toUpperCase()
}

function tryDecode(enc) {
  try {
    const inst = new Instruction(enc)
    return inst.asm
  } catch (e) {
    return null
  }
}

function findNeighbors(enc) {
  const baseInt = parseEncoding(enc)
  const neighbors = []

  for (let bit = 0; bit < 32; bit++) {
    const flippedEnc = flipBit(baseInt, bit)
    const asm = tryDecode(flippedEnc)
    if (asm) {
      neighbors.push({ flippedEnc, bit, asm })
    }
  }

  return neighbors
}

// --- main ---
const inputs = process.argv.slice(2)
if (inputs.length === 0) {
  console.error('Usage: node bitflip.js <encoding1> [encoding2] …')
  process.exit(1)
}

for (const enc of inputs) {
  console.log(`\n=== Neighbors of ${enc} ===`)
  const neigh = findNeighbors(enc)
  if (neigh.length === 0) {
    console.log('  (none)\n')
  } else {
    neigh.forEach(n => {
      console.log(`  flip bit ${n.bit.toString().padStart(2)} → ${n.flippedEnc}  ${n.asm}`)
    })
  }
}
