import { createCanvas } from 'canvas';
import fs from 'fs';
import { Instruction } from 'rvcodecjs/core/Instruction.js';

// === SETTINGS ===
const OUT_DIR = './data';
const IMG_FILE = `${OUT_DIR}/riscv.png`;
const JSON_FILE = `${OUT_DIR}/graph.json`;

const WIDTH = 512;
const HEIGHT = 512;
const TOTAL = WIDTH * HEIGHT;

// === Color palette for classes ===
const classColors = {
  lui: '#4FC3F7',
  auipc: '#81C784',
  jal: '#FFD54F',
  jalr: '#FF8A65',
  beq: '#CE93D8',
  load: '#90CAF9',
  store: '#F48FB1',
  op: '#FFF176',
  'op-imm': '#A1887F',
  misc: '#AED581',
  system: '#B0BEC5',
  float: '#80CBC4',
  compressed: '#BA68C8',
  invalid: '#424242'
};

// === classify based on asm mnemonic ===
function classify(asm) {
  if (!asm) return 'invalid';
  const mnem = asm.split(' ')[0];
  if (mnem.startsWith('c.')) return 'compressed';
  if (mnem.startsWith('f')) return 'float';
  if (['lui', 'auipc', 'jal', 'jalr'].includes(mnem)) return mnem;
  if (['beq', 'bne', 'blt', 'bge', 'bltu', 'bgeu'].includes(mnem)) return 'beq';
  if (['lw', 'lh', 'lb', 'lbu', 'lhu'].includes(mnem)) return 'load';
  if (['sw', 'sh', 'sb'].includes(mnem)) return 'store';
  if (['addi', 'slli', 'slti', 'xori', 'srli', 'srai', 'ori', 'andi'].includes(mnem)) return 'op-imm';
  if (['add', 'sub', 'sll', 'slt', 'sltu', 'xor', 'srl', 'sra', 'or', 'and'].includes(mnem)) return 'op';
  if (['ecall', 'ebreak', 'csrrw', 'csrrs', 'csrrc', 'csrrwi', 'csrrsi', 'csrrci'].includes(mnem)) return 'system';
  return 'misc';
}

// === main ===
async function main() {
  if (!fs.existsSync(OUT_DIR)) {
    fs.mkdirSync(OUT_DIR);
  }

  const canvas = createCanvas(WIDTH, HEIGHT);
  const ctx = canvas.getContext('2d');

  const imgData = ctx.getImageData(0, 0, WIDTH, HEIGHT);
  const data = imgData.data;

  const output = [];

  console.log(`Generating ${TOTAL} instructions...`);
  
  for (let i = 0; i < TOTAL; i++) {
    const insn = i * Math.floor(0xFFFFFFFF / TOTAL);

    let inst;
    try {
      inst = new Instruction(insn.toString(16).padStart(8, '0'));
    } catch {
      inst = null;
    }

    let asm = inst ? inst.asm : null;
    let hex = insn.toString(16).padStart(8, '0');
    let cls = classify(asm);
    let color = classColors[cls] || classColors.invalid;

    // Plot pixel
    const idx = i * 4;
    const rgb = hexToRgb(color);
    data[idx] = rgb.r;
    data[idx + 1] = rgb.g;
    data[idx + 2] = rgb.b;
    data[idx + 3] = 255;

    // Save for graph.json
    output.push({
      hex,
      asm: asm || 'invalid',
      x: i % WIDTH,
      y: Math.floor(i / WIDTH),
      class: cls
    });

    if (i % 50000 === 0) console.log(`Progress: ${(i/TOTAL*100).toFixed(1)}%`);
  }

  ctx.putImageData(imgData, 0, 0);

  const buffer = canvas.toBuffer('image/png');
  fs.writeFileSync(IMG_FILE, buffer);
  fs.writeFileSync(JSON_FILE, JSON.stringify(output, null, 2));

  console.log(`Done! Wrote ${IMG_FILE} and ${JSON_FILE}`);
}

// === Helper ===
function hexToRgb(hex) {
  const bigint = parseInt(hex.slice(1), 16);
  return {
    r: (bigint >> 16) & 255,
    g: (bigint >> 8) & 255,
    b: bigint & 255
  };
}

main();
