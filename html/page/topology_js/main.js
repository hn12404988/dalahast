node_color_list = ["#7FFFD4","#00FFFF","#FFE4C4","#FFD700","#ADFF2F","#FF69B4","#90EE90"];
data_racing_id = "3_data_racing";
show_label_size = 8;
default_size = 4;
lock = false;

function init(){
	ws.send(3,"topology/init",{"init":"1"},function(obj){
		if(obj==false){
			throw new Error ("stop");
		}
		var i,
			g = {
				nodes: [],
				edges: []
			},
			center_x = 100,
			center_y = 100,
			width = 200,
			x,y,l,
			server = [],
			arr = [],
			sub_arr = [],
			subsub_arr = [],
			tmp_arr = [],
			server_index;
		for(i=0;i<obj["info"].length;i++){
			if(server.indexOf(obj["info"][i]["server"])==-1){
				server.push(obj["info"][i]["server"]);
			}
		}
		arr = pie_distribution(center_x,center_y,width,server.length);
		for(i=0;i<server.length;i++){
			build_server_index(parseInt(server[i]));
			sub_arr = [];
			server_index = parseInt(server[i]);
			for(y=0;y<obj["info"].length;y++){
				if(obj["info"][y]["server"]==server[i]){
					if(sub_arr.indexOf(obj["info"][y]["folder"])==-1){
						sub_arr.push(obj["info"][y]["folder"]);
					}
				}
			}
			if(sub_arr.length==0){
				for(y=0;y<obj["info"].length;y++){
					if(obj["info"][y]["server"]==server[i]){
						tmp_arr = get_coordinate(arr[i+1]["x"],arr[i+1]["y"],arr[0]);
						g.nodes.push({
							id: server[i]+"_"+obj["info"][y]["node"],
							label: obj["info"][y]["node"],
							x: tmp_arr[0],
							y: tmp_arr[1],
							size: default_size,
							color: node_color_list[server_index],
						});
						/*
						if(obj["info"][y]["private"]=="0"){
							g.edges.push({
								id: server[i]+'_/control_'+server[i]+"_/"+obj["info"][y]["node"],
								source: server[i]+'_/control',
								target: server[i]+"_/"+obj["info"][y]["node"],
								size: 2,
								color: edge_color("fire"),
								fire_type: "fire",
								type: ['arrow']
							});
						}
						*/
					}
				}
			}
			else{
				subsub_arr = pie_distribution(arr[i+1]["x"],arr[i+1]["y"],2*arr[0],sub_arr.length);
				for(l=0;l<sub_arr.length;l++){
					for(y=0;y<obj["info"].length;y++){
						if(obj["info"][y]["server"]==server[i] && obj["info"][y]["folder"]==sub_arr[l]){
							tmp_arr = get_coordinate(subsub_arr[l+1]["x"],subsub_arr[l+1]["y"],subsub_arr[0]);
							g.nodes.push({
								id: server[i]+"_"+obj["info"][y]["folder"]+"/"+obj["info"][y]["node"],
								label: obj["info"][y]["folder"]+"/"+obj["info"][y]["node"],
								x: tmp_arr[0],
								y: tmp_arr[1],
								size: default_size,
								color: node_color_list[server_index]
							});
							if(obj["info"][y]["private"]=="1"){
								g.nodes[g.nodes.length-1].borderColor = "#000";
								g.nodes[g.nodes.length-1].borderWidth = 2;
							}
							if(g.nodes[g.nodes.length-1]["label"][0]=="/"){
								g.nodes[g.nodes.length-1]["label"] = g.nodes[g.nodes.length-1]["label"].substr(1);
							}
							/*
							if(obj["info"][y]["private"]=="0"){
								g.edges.push({
									id: server[i]+'_control_'+server[i]+"_"+obj["info"][y]["folder"]+"/"+obj["info"][y]["node"],
									source: server[i]+'_control',
									target: server[i]+"_"+obj["info"][y]["folder"]+"/"+obj["info"][y]["node"],
									size: 2,
									color: edge_color("NULL"),
									fire_type: "fire",
									type: ['arrow']
								});
							}
							*/
						}
					}
				}
			}
			//tmp_arr = near_center(arr[i+1]["x"],arr[i+1]["y"],center_x,center_y,arr[0]);
			/*
			g.nodes.push({
				id: server[i]+"_control",
				label: "control",
				x: arr[i+1]["x"],
				y: arr[i+1]["y"],
				size: default_size,
				color: node_color_list[server_index]
			});
			*/
			/*
			tmp_arr = near_center(arr[i+1]["x"],arr[i+1]["y"],center_x,center_y,arr[0]);
			g.nodes.push({
				id: server[i]+"_out",
				label: "out",
				x: tmp_arr[0],
				y: tmp_arr[1],
				size: default_size,
				color: node_color_list[server_index]
			});
			*/
		}
		l = obj["fire"].length;
		y = obj["info"].length;
		for(i=0;i<l;i++){
			server_index = 0;
			for(x=0;x<y;x++){
				if(server_index==2){
					break;
				}
				if(obj["info"][x]["server"]==obj["fire"][i]["from_server"] && obj["info"][x]["folder"]==obj["fire"][i]["from_folder"] && obj["info"][x]["node"]==obj["fire"][i]["from_node"]){
					server_index++;
					continue;
				}
				if(obj["info"][x]["server"]==obj["fire"][i]["to_server"] && obj["info"][x]["folder"]==obj["fire"][i]["to_folder"] && obj["info"][x]["node"]==obj["fire"][i]["to_node"]){
					server_index++;
					continue;
				}
			}
			if(server_index!=2){
				console.log("No node exist in info.");
				console.log(obj["fire"][i]["from_server"]+"_"+obj["fire"][i]["from_folder"]+"/"+obj["fire"][i]["from_node"]);
				console.log(obj["fire"][i]["to_server"]+"_"+obj["fire"][i]["to_folder"]+"/"+obj["fire"][i]["to_node"]);
			}
			else{
				g.edges.push({
					id: obj["fire"][i]["from_server"]+"_"+obj["fire"][i]["from_folder"]+"/"+obj["fire"][i]["from_node"]+"_"+obj["fire"][i]["to_server"]+"_"+obj["fire"][i]["to_folder"]+"/"+obj["fire"][i]["to_node"],
					source: obj["fire"][i]["from_server"]+"_"+obj["fire"][i]["from_folder"]+"/"+obj["fire"][i]["from_node"],
					target: obj["fire"][i]["to_server"]+"_"+obj["fire"][i]["to_folder"]+"/"+obj["fire"][i]["to_node"],
					size: 1,
					color: edge_color(obj["fire"][i]["type"]),
					fire_type: obj["fire"][i]["type"],
					type: ['arrow']
				});
			}
		}
		s = new sigma({
			graph: g,
			renderer: {
				type: 'canvas',
				container: 'graph-container'
			},
			settings: {
				autoRescale: true,
				defaultNodeType: 'border',
				labelThreshold: show_label_size
			}
		}),
		db = new sigma.plugins.neighborhoods();
		db.read(g);
		build_edge_index();
		s.bind('clickNode', function(event) {
			if(lock==true){
				return;
			}
			//if (event.data.node.center==undefined){
			ws.send(3,"topology/node_info",{"node_id":event.data.node.id},function(obj){
				fill_node_info(obj);
			});
			var nodes = s.graph.nodes(),
				edges = s.graph.edges(),
				nb = db.neighborhood(event.data.node.id),
				i,j,index,
				len = nodes.length,
				len2;
			for(i=0;i<len;i++){
				nodes[i]["color"] = "#DCDCDC";
				nodes[i]["size"] = default_size;
			}
			len = edges.length;
			for(i=0;i<len;i++){
				edges[i]["color"] = "#DCDCDC";
			}
			len = nb["nodes"].length;
			len2 = nodes.length;
			for(i=0;i<len;i++){
				index = nb["nodes"][i]["id"].indexOf("_");
				index = nb["nodes"][i]["id"].substr(0,index);
				index = parseInt(index);
				for(j=0;j<len2;j++){
					if(nodes[j]["id"]==nb["nodes"][i]["id"]){
						nodes[j]["color"] = node_color_list[index];
						nodes[j]["size"] = show_label_size;
						break;
					}
				}
			}
			len = nb["edges"].length;
			len2 = edges.length;
			for(i=0;i<len;i++){
				for(j=0;j<len2;j++){
					if(nb["edges"][i]["id"]==edges[j]["id"]){
						edges[j]["color"] = edge_color(edges[j]["fire_type"]);
						break;
					}
				}
			}
			s.refresh();
			//}
		});
		//show_data_racing(document.getElementById("filter_data_racing"));
		s.refresh();
	});
}

function fill_node_info(obj){
	var arr = ["node","node_type","interface","input","input_form","anti_data_racing"],
		i = 0;
	if(obj==false || obj==true){
		for(i=0;i<arr.length;i++){
			document.getElementById(arr[i]).innerHTML = "";
		}
		return;
	}
	for(i=0;i<arr.length;i++){
		document.getElementById(arr[i]).innerHTML = obj[arr[i]];
	}
}

function pie_distribution(X,Y,L,N){
	var arr = [],
		index,i,j,k,l,tmp,inner_r,inner_l;
	i = Math.sin(Math.PI/N);
	arr.push(i*L*0.5/(1+i));
	inner_r = (L*0.5) - arr[0];
	arr.push({
		x: X,
		y: Y - inner_r
	});
	inner_l = 2 * inner_r * Math.sin(Math.PI/N);
	for(index=1;index<arr.length;index++){
		if(X>arr[index]["x"]){
			if(X-arr[index]["x"]<1){
				arr[index]["x"] = X;
			}
		}
		else{
			if(arr[index]["x"]-X<1){
				arr[index]["x"] = X;
			}
		}
		if(Y>arr[index]["y"]){
			if(Y-arr[index]["y"]<1){
				arr[index]["y"] = Y;
			}
		}
		else{
			if(arr[index]["y"]-Y<1){
				arr[index]["y"] = Y;
			}
		}
		if(index==N){
			break;
		}
		i = arr[index]["x"]*arr[index]["x"] + arr[index]["y"]*arr[index]["y"] - X*X - Y*Y;
		i = inner_l*inner_l - inner_r*inner_r - i;
		if(X==arr[index]["x"]){
			j = i/(2*(Y-arr[index]["y"]));
			i = inner_r*inner_r - (j-Y)*(j-Y);
			i = Math.sqrt(i);
			tmp = i;
			i = tmp + X;
			for(k=1;k<arr.length;k++){
				if(Math.abs(arr[k]["x"]-i)<1 && Math.abs(arr[k]["y"]-j)<1){
					break;
				}
			}
			if(k==arr.length){
				arr.push({
					x: i,
					y: j
				}); 
			}
			i = X - tmp;
			for(k=1;k<arr.length;k++){
				if(Math.abs(arr[k]["x"]-i)<1 && Math.abs(arr[k]["y"]-j)<1){
					break;
				}
			}
			if(k==arr.length){
				arr.push({
					x: i,
					y: j
				}); 
			}
		}
		else if(Y==arr[index]["y"]){
			j = i/(2*(X-arr[index]["x"]));
			i = inner_r*inner_r - (j-X)*(j-X);
			i = Math.sqrt(i);
			tmp = i;
			i = tmp + Y;
			for(k=1;k<arr.length;k++){
				if(Math.abs(arr[k]["x"]-j)<1 && Math.abs(arr[k]["y"]-i)<1){
					break;
				}
			}
			if(k==arr.length){
				arr.push({
					x: j,
					y: i
				}); 
			}
			i = Y - tmp;
			for(k=1;k<arr.length;k++){
				if(Math.abs(arr[k]["x"]-j)<1 && Math.abs(arr[k]["y"]-i)<1){
					break;
				}
			}
			if(k==arr.length){
				arr.push({
					x: j,
					y: i
				}); 
			}
		}
		else{
			i = i/(2*(X-arr[index]["x"]));
			tmp = i;
			j = (Y-arr[index]["y"])/(X-arr[index]["x"]);
			i = i - X;
			l = (i*i + Y*Y - inner_r*inner_r)/(1+j*j);
			k = (2*Y + 2*i*j)/(1+j*j);
			l = Math.sqrt(k*k/4 - l) + k*0.5;
			k = tmp - j*l;
			for(i=1;i<arr.length;i++){
				if(Math.abs(arr[i]["x"]-k)<1 && Math.abs(arr[i]["y"]-l)<1){
					break;
				}
			}
			if(i==arr.length){
				arr.push({
					x: k,
					y: l
				}); 
			}
		}
	}
	return arr;
}

function get_coordinate(X,Y,R){
	var arr = [],
		i;
	X = X - R;
	Y = Y - R;
	R = R*2;
	arr.push((X+Math.random()*R));
	arr.push((Y+Math.random()*R));
	return arr;
}

function near_center(x,y,X,Y,r){
	var l,ratio,
		arr = [];
	l = (x-X)*(x-X) + (y-Y)*(y-Y);
	l = Math.sqrt(l);
	ratio = r/l;
	ratio = ratio*0.8;
	arr.push(x+(X-x)*ratio);
	arr.push(y+(Y-y)*ratio);
	arr[0] = arr[0]+Math.random()*5;
	arr[1] = arr[1]+Math.random()*5;
	return arr;
}

function edge_color(type){
	if(type=="fire"){
		return "#1E90FF";
	}
	else if(type=="fireNstore"){
		return "#228B22";
	}
	else if(type=="fireNforget"){
		return "#4B0082";
	}
	else if(type=="fireNclose"){
		return "#20B2AA";
	}
	else if(type=="fireNfreeze"){
		return "#191970";
	}
	else if(type=="fireNcheck"){
		return "#000000";
	}
	else if(type=="NULL"){
		return "#FFFAF0";
	}
	
}

function reset_graph(){
	var nodes = s.graph.nodes(),
		edges = s.graph.edges(),
		i,j,index;
	j = nodes.length;
	for(i=0;i<j;i++){
		index = nodes[i]["id"].indexOf("_");
		index = nodes[i]["id"].substr(0,index);
		index = parseInt(index);
		nodes[i]["color"] = node_color_list[index];
		nodes[i]["size"] = default_size;
	}
	j = edges.length;
	for(i=0;i<j;i++){
		edges[i]["color"] = edge_color(edges[i]["fire_type"]);
	}
	//show_data_racing(document.getElementById("filter_data_racing"));
	s.refresh();
	fill_node_info(true);
}

function build_server_index(index){
	var parent = document.getElementById("ref"),
		div = document.createElement("div");
	div.innerHTML = "Server: "+index;
	div.style = "font-weight: bold;color:white;background-color:"+node_color_list[index]+";";
	parent.appendChild(div);
}

function build_edge_index(){
	var parent = document.getElementById("ref"),
		fire = document.createElement("div"),
		fireNstore = document.createElement("div"),
		fireNforget = document.createElement("div"),
		fireNclose = document.createElement("div"),
		fireNfreeze = document.createElement("div"),
		fireNcheck = document.createElement("div");
	fire.innerHTML = "fire";
	fire.style = "font-weight: bold;color:white;background-color:"+edge_color("fire")+";";
	fireNstore.innerHTML = "fireNstore";
	fireNstore.style = "font-weight: bold;color:white;background-color:"+edge_color("fireNstore")+";";
	fireNforget.innerHTML = "fireNforget";
	fireNforget.style = "font-weight: bold;color:white;background-color:"+edge_color("fireNforget")+";";
	fireNclose.innerHTML = "fireNclose";
	fireNclose.style = "font-weight: bold;color:white;background-color:"+edge_color("fireNclose")+";";
	fireNfreeze.innerHTML = "fireNfreeze";
	fireNfreeze.style = "font-weight: bold;color:white;background-color:"+edge_color("fireNfreeze")+";";
	fireNcheck.innerHTML = "fireNcheck";
	fireNcheck.style = "font-weight: bold;color:white;background-color:"+edge_color("fireNcheck")+";";
	parent.appendChild(fire);
	parent.appendChild(fireNstore);
	parent.appendChild(fireNforget);
	parent.appendChild(fireNclose);
	parent.appendChild(fireNfreeze);
	parent.appendChild(fireNcheck);
}
/*
function show_data_racing(checkbox){
	var	edges = s.graph.edges(),
		i,j;
	j = edges.length;
	if(checkbox==true){
		for(i=0;i<j;i++){
			if(edges[i]["target"]==data_racing_id){
				edges[i]["color"] = edge_color("fire");
			}
		}
		document.getElementById("filter_data_racing").checked = true;
		return;
	}
	if(checkbox==false){
		for(i=0;i<j;i++){
			if(edges[i]["target"]==data_racing_id){
				edges[i]["color"] = edge_color("NULL");
			}
		}
		document.getElementById("filter_data_racing").checked = false;
		return;
	}
	if(checkbox.checked==true){
		for(i=0;i<j;i++){
			if(edges[i]["target"]==data_racing_id){
				edges[i]["color"] = edge_color("fire");
			}
		}
	}
	else{
		for(i=0;i<j;i++){
			if(edges[i]["target"]==data_racing_id){
				edges[i]["color"] = edge_color("NULL");
			}
		}
	}
}
*/

function dragable(checkbox){
	if(checkbox==true){
		sigma.plugins.dragNodes(s, s.renderers[0]);
		document.getElementById("filter_dragable").checked = true;
		return;
	}
	if(checkbox==false){
		sigma.plugins.killDragNodes(s);
		document.getElementById("filter_dragable").checked = false;
		return;
	}
	if(checkbox.checked==true){
		sigma.plugins.dragNodes(s, s.renderers[0]);
	}
	else{
		sigma.plugins.killDragNodes(s);
	}
}

function lock_node(checkbox){
	if(checkbox==true){
		lock = true;
		document.getElementById("filter_lock").checked = true;
		return;
	}
	if(checkbox==false){
		lock = false;
		document.getElementById("filter_lock").checked = false;
		return;
	}
	if(checkbox.checked==true){
		lock = true;
	}
	else{
		lock = false;
	}
}

function side_info(){
	var side = document.getElementById("side"),
		main = document.getElementById("graph-container");
	if(side.hidden==true){
		side.hidden = false;
		main.style.width = "80%";
	}
	else{
		side.hidden = true;
		main.style.width = "100%";
	}
	s.refresh();
}

function search_node(input){
	var i,j;
	i = input.indexOf(" ");
	if(i==-1){
		return;
	}
	var server = input.substr(0,i),
		node = input.substr(i+1),
		nodes = s.graph.nodes(),
		len = server.length+node.length+1,
		id = server+"_"+node;
	j = nodes.length;
	for(i=0;i<j;i++){
		if(nodes[i]["id"].substr(0,len)==id){
			s.cameras[0].goTo({x:nodes[i][s.cameras[0].readPrefix + 'x'],y:nodes[i][s.cameras[0].readPrefix + 'y'],ratio:0.3});
			break;
		}
	}
}
