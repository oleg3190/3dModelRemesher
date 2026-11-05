import fs from 'fs';
let src = fs.readFileSync('/home/s/Загрузки/AstraForge-update-0.5.3/node_modules/meshoptimizer/meshopt_simplifier.js','utf8');
src = src.replace(/export\s*\{[^}]*\}\s*;?/g, '');
const patched = src.replace('WebAssembly.instantiate(unpack(wasm), {})', 'capture(unpack(wasm))');
if (patched === src) throw new Error('patch failed');
globalThis.__cap = null;
function capture(buf){ globalThis.__cap = buf; return { then(){ return this; } }; }
eval(patched);
const bytes = Buffer.from(globalThis.__cap);
fs.writeFileSync('/tmp/meshopt-an/meshopt_simplifier.wasm', bytes);
console.log('wasm bytes:', bytes.length, 'magic:', bytes.slice(0,8).toString('hex'));
