function init(){
	request("init");
}

function refresh_up_tag(name,amount){
	var ele = document.getElementById(name),
		parent = document.getElementById("up");
	if(ele==undefined){
		var ele = document.createElement("input");
		ele.type = "button";
		ele.setAttribute("class","up_block");
		ele.setAttribute("id",name);
		ele.value = name+" ("+amount+")";
		ele.setAttribute("onclick","request(this.id)");
		parent.appendChild(ele);
	}
	else{
		ele.value = name+" ("+amount+")";
	}
	/*
	if(amount=="0"){
		ele.style.display = "none";
	}
	*/
}

function request(index){
	parent.ws.send(0,"error_log/request",{"server":"all","up_tag":index},function(obj){
		if(obj==false){
			throw new Error("stop");
		}
		var i,
			tag_index = -1,
			parent = document.getElementById("down"),
			name = [];
		for(i=0;i<obj["up_tag"].length;i++){
			if(index==obj["up_tag"][i]){
				tag_index = i;
			}
			refresh_up_tag(obj["up_tag"][i],obj["amount"][i]);
		}
		if(tag_index==-1){
			if(obj["amount"][0]==0){
				return;
			}
		}
		else{
			if(obj["amount"][tag_index]==0){
				return;
			}
		}
		for(var ele in obj["content"]){
			name = Object.keys(obj["content"][ele][0]);
			break;
		}
		parent.children[0].innerHTML = "";
		if(name.length>1){
			build_column(name);
			for(var ele in obj["content"]){
				for(i=0;i<obj["content"][ele].length;i++){
					build_row(ele,name,obj["content"][ele][i]);
				}
			}
		}
	});
}

function build_column(name){
	var i = 0,
		table = document.getElementById("down").children[0],
		tr = document.createElement("tr");
	var th = document.createElement("th");
	th.innerHTML = "server";
	tr.appendChild(th);
	for(i=0;i<name.length;i++){
		var th = document.createElement("th");
		th.innerHTML = name[i];
		tr.appendChild(th);
	}
	table.appendChild(tr);
}

function build_row(server,name,obj){
	var table = document.getElementById("down").children[0],
		tr = document.createElement("tr"),
		i = 0;
	var	td = document.createElement("td");
	td.innerHTML = server;
	td.name = "server";
	tr.appendChild(td);
	for(i=0;i<name.length;i++){
		var	td = document.createElement("td");
		td.innerHTML = obj[name[i]];
		td.name = name[i];
		tr.appendChild(td);
	}
	table.appendChild(tr);
}
