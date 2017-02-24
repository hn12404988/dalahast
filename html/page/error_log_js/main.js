now_up_tag = "";

function init(){
	ws.send(0,"error_log/request",{"up_tag":"init"},function(obj){
		if(obj==false){
			throw new Error("stop");
		}
		var i;
		for(i=0;i<obj["up_tag"].length;i++){
			if(i==0){
				now_up_tag = obj["up_tag"][i];
			}
			build_up_tag(obj["up_tag"][i],obj["amount"][i]);
		}
	});
}

function build_up_tag(name,amount){
	var button = document.createElement("input"),
		parent = document.getElementById("up");
	button.type = "button";
	button.setAttribute("class","up_block");
	button.setAttribute("id",name);
	button.value = name+" ("+amount+")";
	button.setAttribute("onclick","request(this.id)");
	/*
	if(amount=="0"){
		button.style.display = "none";
	}
	*/
	parent.appendChild(button);
}

function refresh_up_tag(name,amount){
	document.getElementById(name).value = name+" ("+amount+")";
}

function request(index){
	ws.send(0,"error_log/request",{"up_tag":index},function(obj){
		if(obj==false){
			throw new Error("stop");
		}
		now_up_tag = index;
		var i,
			parent = document.getElementById("down"),
			name = [];
		for(i=0;i<obj["up_tag"].length;i++){
			refresh_up_tag(obj["up_tag"][i],obj["amount"][i]);
		}
		name = Object.keys(obj["content"][0]);
		parent.children[0].innerHTML = "";
		if(name.length>0){
			for(i=0;i<obj["content"].length;i++){
				if(i==0){
					build_column(obj["content"][i]);
				}
				build_row(name,obj["content"][i]);
			}
		}
	});
}

function build_column(obj){
	var i = 0,
		table = document.getElementById("down").children[0],
		tr = document.createElement("tr"),
		name = Object.keys(obj);
	for(i=0;i<name.length;i++){
		var th = document.createElement("th");
		th.innerHTML = name[i];
		tr.appendChild(th);
	}
	/*
	var th = document.createElement("th");
	th.innerHTML = "Admin";
	tr.appendChild(th);
	*/
	table.appendChild(tr);
}

function build_row(name,obj){
	var table = document.getElementById("down").children[0],
		tr = document.createElement("tr"),
		i = 0;
	for(i=0;i<name.length;i++){
		var	td = document.createElement("td");
		td.innerHTML = obj[name[i]];
		td.name = name[i];
		tr.appendChild(td);
	}
	/*
	var	td = document.createElement("td");
	td.name = "admin";
	td.innerHTML = "Delete";
	td.setAttribute("onclick","delete_data(this.parentElement)");
	tr.appendChild(td);
	*/
	table.appendChild(tr);
}

/*
function delete_data(ele){
	var children = ele.children,
		i,
		id,
		obj = {};
	for(i=0;i<children.length;i++){
		if(children[i].name=="id"){
			id = children[i].innerHTML;
		}
	}
	obj["up_tag"] = now_up_tag;
	obj["name"] = getCookie("staff_name");
	obj["password"] = getCookie("password");
	obj["id"] = id;
	ws.send(0,"error_log/delete",obj,function(obj2){
		if(obj2==false){
			throw new Error("stop");
		}
		refresh_up_tag(now_up_tag,obj2["amount"]);
		ele.parentElement.removeChild(ele);
	});
}
*/
