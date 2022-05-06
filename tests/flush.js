let density = 16;


let mass = 10000;
let distance = 50;

for (let i = 0; i < 32; i++) {
    console.log(`${fixed(distance - Math.random() * distance * 2)} ${fixed(distance - Math.random() * distance* 2)} ${fixed(distance - Math.random() * distance* 2)} 0 0 0 ${((Math.random() * mass + mass) 
   + mass)}`);
}

function fixed(x) {
    return Number.parseFloat(x).toFixed(16);
}