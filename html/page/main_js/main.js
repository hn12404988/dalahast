function screen(){
	var main = document.getElementById("screen_1");
	if(main==undefined){
		var main = document.getElementById("screen_2");
	}
	this.dom = {};
	this.dom.main = main;
	this.left_right_close = true;
}

screen.prototype.left_right = function(){
	if(this.left_right_close==true){
		_screen.dom.main.id = "screen_2";
		this.left_right_close = false;
	}
	else{
		_screen.dom.main.id = "screen_1";
		this.left_right_close = true;
	}
}

screen.prototype.side = function(){
	if(_side.dom.main.hidden==false){
		_screen.left_right();
	}
	else{
		_side.dom.main.hidden = false;
		_screen.left_right();
	}
}

function side(){
	var main = document.getElementById("side");
	this.dom = {};
	this.dom.main = main;
}
