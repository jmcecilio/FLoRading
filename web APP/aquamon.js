var options_msg_chart;
var options1;
var msg_chart;
var energy_chart;
var data_chart
var rssi_chart;


      //var marker;
var markers = []; //Global Marker Array
var energy_values = [];
var messages = [];
var series_rssi = [];
var series_data = [];
var beaches = [];
var pos=0;

function readTextFile(file, callback) {
    var rawFile = new XMLHttpRequest();
    rawFile.overrideMimeType("application/json");
    rawFile.open("GET", file, true);
    rawFile.onreadystatechange = function() {
        if (rawFile.readyState === 4 && rawFile.status == "200") {
            callback(rawFile.responseText);
        }
    }
    rawFile.send(null);
}



    function compare( a, b ) {
        if ( moment(a["inserted_at"]) < moment(b["inserted_at"]) ){
            return -1;
        }
        if ( moment(a["inserted_at"]) > moment(b["inserted_at"])){
            return 1;
        }
        return 0;
        }

//var series_time_ref = ["08:20","08:40","09:00","09:20","09:40","10:00","10:20","10:40","11:00","11:20","11:40","12:00","12:20","12:40","13:00","13:20","13:40","14:00","14:20","14:40","15:00","15:20","15:40","16:00","16:20","16:40","17:00","17:20","17:40","18:00","18:20","18:40","19:00","19:20","19:40","20:00","20:20","20:40","21:00","21:20","21:40","22:00","22:20","22:40","23:00","23:20","23:40","00:00","00:20","00:40","01:00","01:20","01:40","02:00","02:20","02:40","03:00","03:20","03:40","04:00","04:20","04:40","05:00","05:20","05:40","06:00","06:20","06:40","07:00","07:20","07:40","08:00","08:20","08:40"];

var series_time_ref = ["08:20"," "," "," "," "," ","10:20"," "," "," "," "," ","12:20","","","","","","14:20","","","","","","16:20","","","","","","18:20","","","","","","20:20","","","","","","22:20","","","","","","00:20","","","","","","02:20","","","","","","04:20","","","","","","06:20","","","","","","08:20",""];


function getMessageTuple(index) {
	return messages[index].data;
}

function getEnergyValues(index) {
	if (index == 1000)
		return energy_values;
	else
		return energy_values[index];
}


     function initMap() {}

     function updateMapWithLocations() {
        var map = new google.maps.Map(document.getElementById('map'), {
          zoom: 13,
          center: {lat: 38.62901, lng: -9.11}
        });

	for (var i = 0; i < beaches.length; i++) {
		var beach = beaches[i];
		marker = new google.maps.Marker({
		  map: map,
		  draggable: true,
		  animation: google.maps.Animation.DROP,
		  position: {lat: beach[1], lng: beach[2]},
		  title: "Node "+ i+"",
		  icon: 'http://maps.google.com/mapfiles/ms/icons/green-dot.png'
		});
	    
	    markers.push(marker);
	}


	markers[0].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[0]);
        });

	markers[1].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[1]);
        });

	markers[2].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[2]);
        });

	markers[3].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[3]);
        });
        
	markers[4].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[4]);
        });

	markers[5].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[5]);
        });

	markers[6].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[6]);
        });


	markers[7].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[7]);
        });

	markers[8].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[8]);
        });

	markers[9].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[9]);
        });
        
	markers[10].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[10]);
        });

	markers[11].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[11]);
        });

	markers[12].addListener('click', function(e) {
          placeMarkerAndPanTo(e.latLng, markers[12]);
        });


	markers[0].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[0];
                markers[0].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[1].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[1];
                markers[1].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[2].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[2];
                markers[2].setPosition({lat: beach[1], lng: beach[2]});
        });

	markers[3].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[3];
                markers[3].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[4].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[4];
                markers[4].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[5].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[5];
                markers[5].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[6].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[6];
                markers[6].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[7].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[7];
                markers[7].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[8].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[8];
                markers[8].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[9].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[9];
                markers[9].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[10].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[10];
                markers[10].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[11].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[11];
                markers[11].setPosition({lat: beach[1], lng: beach[2]});
        });
	markers[12].addListener( 'dragend', function(e) 
        {		
		var beach = beaches[12];
                markers[12].setPosition({lat: beach[1], lng: beach[2]});
        });



      }


function resetMap(){
	markers[0].setAnimation(null);
	markers[1].setAnimation(null);
	markers[2].setAnimation(null);
	markers[3].setAnimation(null);
	markers[4].setAnimation(null);
	markers[5].setAnimation(null);
	markers[6].setAnimation(null);
	markers[7].setAnimation(null);
	markers[8].setAnimation(null);
	markers[9].setAnimation(null);
	markers[10].setAnimation(null);
	markers[11].setAnimation(null);
	markers[12].setAnimation(null);
	markers[0].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[1].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[2].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[3].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[4].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[5].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[6].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[7].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[8].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[9].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[10].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[11].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');
	markers[12].setIcon('http://maps.google.com/mapfiles/ms/icons/green-dot.png');

}

function placeMarkerAndPanTo(latLng, marker) {

	resetMap();

	marker.setAnimation(google.maps.Animation.BOUNCE);
	marker.setIcon('http://maps.google.com/mapfiles/ms/icons/red-dot.png');
	$(".entity-list li").each(function(){
                $(this).css("transform", "scale(1)");
                $(this).css("background", "white");
            })


	if (marker.title == 'Node 0'){

		data_chart.updateSeries( series_data );
		rssi_chart.updateSeries( series_rssi );

		energy_chart.updateSeries(energy_values); 
		msg_chart.updateSeries([{data: getMessageTuple(0) }]);

            $('[name=Gateway10]').css("transform", "scale(1.02)");
            $('[name=Gateway10]').css("background", "rgba(50, 70, 168,0.1)");
	}
	else if (marker.title == 'Node 7'){

			data_chart.updateSeries( series_data );
			rssi_chart.updateSeries( series_rsii );

			energy_chart.updateSeries(energy_values); 
			msg_chart.updateSeries([{data: getMessageTuple(7) }]);

		    $('[name=Gateway20]').css("transform", "scale(1.02)");
		    $('[name=Gateway20]').css("background", "rgba(50, 70, 168,0.1)");
	}
	else {
		if (marker.title == 'Node 1'){
			device = "Node 11";
			device_name = "Node11";
			device_index = 1;
		}
		else if (marker.title == 'Node 2'){
			device = "Node 12";
			device_name = "Node12";
			device_index = 2;
		}
		else if (marker.title == 'Node 3'){
			device = "Node 13";
			device_name = "Node13";
			device_index = 3;
		}
		else if (marker.title == 'Node 4'){
			device = "Node 14";
			device_name = "Node14";
			device_index = 4;
		}
		else if (marker.title == 'Node 5'){
			device = "Node 15";
			device_name = "Node15";
			device_index = 5;
		}
		else if (marker.title == 'Node 6'){
			device = "Node 16";
			device_name = "Node16";
			device_index = 6;
		}


		else if (marker.title == 'Node 8'){
			device = "Node 21";
			device_name = "Node21";
			device_index = 8;
		}
		else if (marker.title == 'Node 9'){
			device = "Node 22";
			device_name = "Node22";
			device_index = 9;
		}
		else if (marker.title == 'Node 10'){
			device = "Node 23";
			device_name = "Node23";
			device_index = 10;
		}
		else if (marker.title == 'Node 11'){
			device = "Node 24";
			device_name = "Node24";
			device_index = 11;
		}
		else if (marker.title == 'Node 12'){
			device = "Node 25";
			device_name = "Node25";
			device_index = 12;
		}

		//renderchart(series.filter(x => x.name == device), "Water level per hour - "+device, "#realtime-chart", "line")
		//renderchart(series.filter(x => x.name == device), "RSSI - "+device, "#rssi-chart", "area")

		data_chart.updateSeries( series_data.filter(x => x.name == device) );
		rssi_chart.updateSeries( series_rssi.filter(x => x.name == device) );

		energy_chart.updateSeries([energy_values[device_index]]);
		msg_chart.updateSeries([{data: getMessageTuple(device_index) }]);
		$('[name=' + device_name + ']').css("transform", "scale(1.02)");
		$('[name=' + device_name + ']').css("background", "rgba(50, 70, 168,0.1)");
	}
      }



    $(document).ready(function(){


	readTextFile("data.json", function(text){
	var data = JSON.parse(text);
	var pos = 0; 
	for (i=0; i<data.members.length; i++){
		energy_values [pos] = 100;
		beaches [pos] = [data.members[i].Network.Gateway.Name, data.members[i].Network.Gateway.lat, data.members[i].Network.Gateway.long ];
		series_rssi [pos] = {
					name: data.members[i].Network.Gateway.Name,
					data: []
				}
		series_data [pos] = {
					name: data.members[i].Network.Gateway.Name,
					data: []
				}
		pos ++;
		for (j=0; j<data.members[i].Network.Gateway.members.length; j++) {
			energy_values [pos] = data.members[i].Network.Gateway.members[j].Node.energy;
			beaches [pos] = [data.members[i].Network.Gateway.members[j].Node.Name, data.members[i].Network.Gateway.members[j].Node.lat, data.members[i].Network.Gateway.members[j].Node.long ];
			series_rssi [pos] = {
					name: data.members[i].Network.Gateway.members[j].Node.Name,
					data: data.members[i].Network.Gateway.members[j].Node.rssi
						}
			series_data [pos] = {
					name: data.members[i].Network.Gateway.members[j].Node.Name,
					data: data.members[i].Network.Gateway.members[j].Node.sensor
						}
			messages [pos] = {
					name: data.members[i].Network.Gateway.members[j].Node.Name,
					data: data.members[i].Network.Gateway.members[j].Node.messages
					}
			pos++;
		}


	}
	messages [0] = {
				name: "Gateway 20",
				data: [823,850,836,799,759,715,648,595,567,525,448,450,373,324,303,273,290,256,231,236,234,225,228,231,234,257,329,410,529,530,534,676,817,852,840,861,864,823,850,836,799,759,715,648,595,567,525,448,450,373,324,303,273,290,256,231,236,234,225,228,231,234,257,329,410,529,530,534,676,817,852,840,861,864],

			}

	messages [7] = {
				name: "Gateway 10",
				data:[748,770,758,665,584,518,408,366,337,290,296,299,247,218,225,195,213,180,156,160,155,150,153,154,156,152,202,259,301,300,303,485,702,774,761,782,786,748,770,758,665,584,518,408,366,337,290,296,299,247,218,225,195,213,180,156,160,155,150,153,154,156,152,202,259,301,300,303,485,702,774,761,782,786],
			}

	updateMapWithLocations();
	int_charts();





        $(".group-list li").click(function(){
            let device = $($($(this).children()[0]).children()[0]).text();

	if (device == "East" || device == "Gateway 20") {
		//renderchart(series, "Water level per hour", "#realtime-chart", "line")
		//renderchart(series, "RSSI", "#rssi-chart", "area")
		data_chart.updateSeries( series_data );
		rssi_chart.updateSeries( series_rssi );

		energy_chart.updateSeries(energy_values); 
		msg_chart.updateSeries([{data: getMessageTuple(0) }]);

		resetMap();

		//markers[7].setLabel("I'm the Gateway");

		markers[7].setAnimation(google.maps.Animation.BOUNCE);
		markers[7].setIcon('http://maps.google.com/mapfiles/ms/icons/red-dot.png');
		

	} else if (device == "West" || device == "Gateway 10") {
		//renderchart(series, "Water level per hour", "#realtime-chart", "line")
		//renderchart(series, "RSSI", "#rssi-chart", "area")
		data_chart.updateSeries( series_data );
		rssi_chart.updateSeries( series_rssi );

		energy_chart.updateSeries(energy_values); 
		msg_chart.updateSeries([{data: getMessageTuple(7) }]);

		resetMap();

		markers[0].setAnimation(google.maps.Animation.BOUNCE);
		markers[0].setIcon('http://maps.google.com/mapfiles/ms/icons/red-dot.png');
		

	} else {

			if (device == 'Node 11'){
				nodeIndex = 1;
			}
			else if (device == 'Node 12'){
				nodeIndex = 2;
			}
			else if (device == 'Node 13'){
				nodeIndex = 3;
			}
			else if (device == 'Node 14'){
				nodeIndex = 4;
			}
			else if (device == 'Node 15'){
				nodeIndex = 5;
			}
			else if (device == 'Node 16'){
				nodeIndex = 6;
			}

			else if (device == 'Node 21'){
				nodeIndex = 8;
			}
			else if (device == 'Node 22'){
				nodeIndex = 9;
			}
			else if (device == 'Node 23'){
				nodeIndex = 10;
			}
			else if (device == 'Node 24'){
				nodeIndex = 11;
			}
			else if (device == 'Node 25'){
				nodeIndex = 12;
			}

			//renderchart(series.filter(x => x.name == device), "Water level per hour - "+device, "#realtime-chart", "line")
            		//renderchart(series.filter(x => x.name == device), "RSSI - "+device, "#rssi-chart", "area")
			data_chart.updateSeries( series_data.filter(x => x.name == device) );
			rssi_chart.updateSeries( series_rssi.filter(x => x.name == device) );

			energy_chart.updateSeries([energy_values[nodeIndex]]);
			msg_chart.updateSeries([{data: getMessageTuple(nodeIndex) }]);
			resetMap();

			markers[nodeIndex].setAnimation(google.maps.Animation.BOUNCE);
			markers[nodeIndex].setIcon('http://maps.google.com/mapfiles/ms/icons/red-dot.png');

		}
            
        })
})

        $(".entity-list li").click(function(){
            console.log("this");
            $(".entity-list li").each(function(){
                $(this).css("transform", "scale(1)");
                $(this).css("background", "white");
            })
            $(this).css("transform", "scale(1.02)");
            $(this).css("background", "rgba(50, 70, 168,0.1)");
            let name = $($($(this).children()[0]).children()[0]).text();
            console.log(name);
            $("#current-display").text(name);
            let group= $(this).attr('name')
            $("ul[name='"+group+"']").slideToggle();
        });

        $("#sample6").on("keyup", function() {
            var value = $(this).val().toLowerCase();
            $(".device-list li").filter(function() {
            $(this).toggle($(this).text().toLowerCase().indexOf(value) > -1)
            });
        });

        $("#interval-picker").change(function(){
            render_chart($(this).val());
        })
    })

   

    

function buildChartOPtions(chart, yAxisTitle, data) {
        var options_chart = {
                chart: {
                    height: 350,
                    type: chart,
                    zoom: {
                        enabled: false
                    }
                },
                series: data,
                dataLabels: {
                    enabled: false
                },
                stroke: {
                    curve: 'straight'
                },

                grid: {
                    row: {
                        colors: ['#f3f3f3', 'transparent'], // takes an array which will be repeated on columns
                        opacity: 0.5
                    },
                },
                xaxis: {
                    categories: series_time_ref,
                    labels: {
                        show: true,
                        rotate: -45,
                        rotateAlways: true,
                        hideOverlappingLabels: true,
                        showDuplicates: false,
                    },
		    title: {
                        text: "Time",
                    },
                },
		yaxis: {
		    title: {
                        text: yAxisTitle,
                    },
                },
                stroker: {
                    width: 0.1
                }
            }
	return options_chart;
}

function int_charts() {

	    options_msg_chart = {
		    chart: {
		        height: 350,
		        type: 'bar',
		    },
		    plotOptions: {
		        bar: {
		            horizontal: false,
		            columnWidth: '50%',
		            endingShape: 'rounded'	
		        },
		    },
		    dataLabels: {
		        enabled: false
		    },
		    stroke: {
		        show: true,
		        width: 2,
		        colors: ['transparent']
		    },
		    series: [{
		        name: "Messages",
		        data: getMessageTuple(0)
		    }],
		    xaxis: {
		        categories: series_time_ref,
			title: {
		            text: 'Time'
		        },
			labels: {
		                show: true,
		                rotate: -90,
		                rotateAlways: false,
		                hideOverlappingLabels: true,
		                showDuplicates: false,
		            },
		    },
		    yaxis: {
		        title: {
		            text: 'Messages per Hour'
		        }
		    },
		    fill: {
		        opacity: 1

		    },
		    tooltip: {
		        y: {
		            formatter: function (val) {
		                return val + ""
		            }
		        }
		    }
		}
		
	    

	    msg_chart = new ApexCharts(
		document.querySelector("#bar-chart"),
		options_msg_chart
	    );
	    msg_chart.render();




	options1 = {
	  chart: {
	    height: 380,
	    type: "radialBar",
	  },

	  series: getEnergyValues(1000),
	  plotOptions: {
	    radialBar: {
	      dataLabels: {
		total: {
		  show: true,
		  label: 'Energy'
		}
	      }
	    },
	  },
	  labels: ['Gateway 10', 'Node 11', 'Node 12', 'Node 13', 'Node 14', 'Node 15', 'Node 16','Gateway 20', 'Node 21', 'Node 22', 'Node 23', 'Node 24', 'Node 25']
	};



	energy_chart = new ApexCharts(document.querySelector("#energy-chart"), options1);
	energy_chart.render();

	//console.log(series_data);

	data_chart = new ApexCharts(
                document.querySelector("#realtime-chart"),
                buildChartOPtions("line", "Water level", series_data)
            );

            data_chart.render();

	rssi_chart = new ApexCharts(
                document.querySelector("#rssi-chart"),
                buildChartOPtions("area", "RSSI", series_rssi)
            );

            rssi_chart.render();

}




   
