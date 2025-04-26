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

// === Dynamic color generation ===
const seenClasses = new Map();
function getColor(cls) {
  if (seenClasses.has(cls)) return seenClasses.get(cls);

  const hue = seenClasses.size * 47 % 360;
  const color = `hsl(${hue}, 60%, 65%)`;
  seenClasses.set(cls, color);
  return color;
}

// === Classification ===
function classify(inst) {
  if (!inst || inst.asm === 'invalid') return 'invalid';
  const isa = inst.isa || 'unknown';
  const fmt = inst.fmt || 'none';
  return `${isa}+${fmt}`;
}

// === Main ===
async function main() {
  if (!fs.existsSync(OUT_DIR)) fs.mkdirSync(OUT_DIR);

  const canvas = createCanvas(WIDTH, HEIGHT);
  const ctx = canvas.getContext('2d');
  const imgData = ctx.getImageData(0, 0, WIDTH, HEIGHT);
  const data = imgData.data;

  const output = [];

  console.log(`Generating ${TOTAL} instructions...`);

  for (let i = 0; i < TOTAL; i++) {
    const insn = i * Math.floor(0xFFFFFFFF / TOTAL);
    const hex = insn.toString(16).padStart(8, '0');

    let inst;
    try {
      inst = new Instruction(hex);
    } catch {
      inst = null;
    }

    const cls = classify(inst);
    const color = getColor(cls);

    // Plot pixel
    const idx = i * 4;
    const rgb = hexToRgb(color);
    data[idx] = rgb.r;
    data[idx + 1] = rgb.g;
    data[idx + 2] = rgb.b;
    data[idx + 3] = 255;

    // Save entry
    output.push({
      hex,
      asm: inst?.asm || 'invalid',
      isa: inst?.isa || 'invalid',
      fmt: inst?.fmt || 'invalid',
      class: cls,
      x: i % WIDTH,
      y: Math.floor(i / WIDTH),
    });

    if (i % 50000 === 0) {
      console.log(`Progress: ${(i / TOTAL * 100).toFixed(1)}%`);
    }
  }

  ctx.putImageData(imgData, 0, 0);
  fs.writeFileSync(IMG_FILE, canvas.toBuffer('image/png'));
  fs.writeFileSync(JSON_FILE, JSON.stringify(output, null, 2));

  console.log(`✅ Wrote ${IMG_FILE} and ${JSON_FILE}`);
}

// === Helpers ===
function hexToRgb(hex) {
  const canvas = createCanvas(1, 1);
  const ctx = canvas.getContext('2d');
  ctx.fillStyle = hex;
  ctx.fillRect(0, 0, 1, 1);
  const [r, g, b] = ctx.getImageData(0, 0, 1, 1).data;
  return { r, g, b };
}

main();
