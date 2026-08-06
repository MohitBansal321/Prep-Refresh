const obj = {
    name: "Mohit"
};

rename(obj);
rename2(obj);   // does not change reassignment doesn't work


function rename(obj) {
    obj.name = "Mohit Bansal";
}

function rename2(obj) {
    obj = {
        name: "Mohit Bansal 2"
    }
}


console.log(obj);