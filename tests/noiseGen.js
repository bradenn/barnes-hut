let SimplexNoise = require('simplex-noise'),
	simplex = new SimplexNoise(Math.random());
for(let i = -256; i < 256; i++){
    for(let j = -256; j < 256; j++) {
        let a = 1024, b = 1024, c = 1024;
        let d = 1000;

        let x = i;
        let y = j;
        let z = 0
        let nz = simplex.noise2D(x, y);
        // console.log(nz)
        if (nz > 0.7 && nz < 0.9) {
            console.log(`${x} ${y} ${z} 0 0 0 ${Math.random() * d}`);
        }
    }
}
