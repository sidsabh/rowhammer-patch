import { Instruction } from '../rvcodecjs/core/Instruction.js';

// export Instruction class

export { Instruction };


// Load canvas
const c = document.getElementById('canvas');
const ctx = c.getContext('2d');
const img = new Image();
img.src = './data/riscv-small.png';
img.onload = () => {
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(img, 0, 0, c.width, c.height);
};

const zoom = Panzoom(c, {
    maxScale: 20,
    cursor: ''
});
c.addEventListener('wheel', zoom.zoomWithWheel);

c.onclick = ev => {
    const { x, y } = getMousePos(c, ev);
    mouseClick(x, y);
};

c.onmousemove = ev => {
    const { x, y } = getMousePos(c, ev);
    mouseHover(x, y);
};

function getMousePos(canvas, ev) {
    const rect = canvas.getBoundingClientRect();
    return {
        x: Math.floor((ev.clientX - rect.left) / (rect.right - rect.left) * canvas.width),
        y: Math.floor((ev.clientY - rect.top) / (rect.bottom - rect.top) * canvas.height)
    };
}

function xyToAddress(x, y) {
    // Map (x, y) linearly to 32-bit space
    const index = y * c.width + x;
    const address = Math.floor(index * (0xFFFFFFFF / (c.width * c.height)));
    return address >>> 0;
}

function decodeInstruction(addr) {
    try {
        const hex = addr.toString(16).padStart(8, '0');
        const inst = new Instruction(hex);
        return inst.asm || 'invalid';
    } catch (e) {
        return 'invalid';
    }
}

function mouseHover(x, y) {
    const addr = xyToAddress(x, y);
    const asm = decodeInstruction(addr);
    document.getElementById('hoverbox').textContent = `0x${addr.toString(16).padStart(8, '0')}: ${asm}`;
}

function mouseClick(x, y) {
    let out = '';
    const center = xyToAddress(x, y);
    for (let delta = -5; delta <= 5; delta++) {
        const addr = (center + delta) >>> 0;
        const asm = decodeInstruction(addr);
        out += `0x${addr.toString(16).padStart(8, '0')}: ${asm}\n`;
    }
    document.getElementById('codebox').textContent = out;
}
