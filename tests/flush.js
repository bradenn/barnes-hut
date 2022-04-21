let density = 16;


let distance = 1000
let mass = 10000

for (let i = 0; i < 1024; i++) {
    console.log(`${fixed(distance - Math.random() * distance * 2)} ${fixed(distance - Math.random() * distance* 2)} ${fixed(distance - Math.random() * distance* 2)} 0 0 0 ${25 + (Math.random() * mass)}`);
}

function fixed(x) {
    return Number.parseFloat(x).toFixed(10);
}