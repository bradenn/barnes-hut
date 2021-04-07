// let SimplexNoise = require('simplex-noise'),
// 	simplex = new SimplexNoise(Math.random());
// for(let i = -256; i < 256; i++){
//     for(let j = -256; j < 256; j++) {
//         let a = 1024, b = 1024, c = 1024;
//         let d = 1000;
//
//         let x = i;
//         let y = j;
//         let z = 0
//         let nz = simplex.noise2D(x, y);
//         // console.log(nz)
//         if (nz > 0.7 && nz < 0.9) {
//             console.log(`${x} ${y} ${z} 0 0 0 ${Math.random() * d}`);
//         }
//     }
// }

let jimp = require('jimp');
function map_range(value, low1, high1, low2, high2) {
    return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}
jimp.read('./cmb.jpeg', (err, cmb) => {
    if (err) throw err;

    for(let i = 0; i < 512; i++){
        for(let j = 0; j < 512; j++) {
            if(i % 2 === 0 && j % 2 === 0){
            let rgba = jimp.intToRGBA(cmb.getPixelColor(i, j))
            console.log(`${(256-(i)) * 4} ${(256-(j)) * 4} 0 0 0 0 ${(rgba.b + rgba.g + rgba.r)}`)
            }
        }
    }


});