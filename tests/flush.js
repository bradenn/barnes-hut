let density = 16;

for (let i = -density; i < density; i++) {
    for (let j = -density; j < density; j++) {
        console.log(`${fixed(i * density)} ${fixed(j * density)} 0 0 0 0 ${Math.random() * 100 + 1000}`);
    }
}

function fixed(x) {
    return Number.parseFloat(x).toFixed(10);
}