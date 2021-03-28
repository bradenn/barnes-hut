for(let i = -256; i < 256; i++){
    for(let j = -256; j < 256; j++){
        if(i % 8 === 0 && j % 8 === 0){
            console.log(`${i} ${j} 0 0 0 0 ${Math.abs(i) + Math.abs(j) + 1}`);
        }
    }
}