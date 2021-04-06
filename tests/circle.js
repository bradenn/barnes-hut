for (let j = 16; j < 256; j++) {
    for (let i = 0; i < 2 * Math.PI; i += (2 * Math.PI) / 256) {
        if (j % 8 === 0) {
            console.log(`${fixed(j * Math.cos(i))} ${fixed(j * Math.sin(i))} 0 0 0 0 ${Math.random() * 1000}`)
        }
    }
}
function fixed(x) {
    return Number.parseFloat(x).toFixed(2);
}