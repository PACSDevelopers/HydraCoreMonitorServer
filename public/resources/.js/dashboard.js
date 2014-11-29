'use strict';

var closestColor = (function () {
    function dist(s, t) {
        if (!s.length || !t.length) return 0;
        return dist(s.slice(2), t.slice(2)) +
        Math.abs(parseInt(s.slice(0, 2), 16) - parseInt(t.slice(0, 2), 16));
    }

    return function (arr, str) {
        var min = 0xffffff;
        var best, current, i;
        for (i = 0; i < arr.length; i++) {
            current = dist(arr[i], str)
            if (current < min) {
                min = current
                best = arr[i];
            }
        }
        return best;
    };
}());

var colorMap = {
    '191970': 'midnightblue',
    '696969': 'dimgray',
    '708090': 'slategray',
    '778899': 'lightslategray',
    '800000': 'maroon',
    '800080': 'purple',
    '808000': 'olive',
    '808080': 'gray',
    'f0f8ff': 'aliceblue',
    'faebd7': 'antiquewhite',
    '00ffff': 'cyan',
    '7fffd4': 'aquamarine',
    'f0ffff': 'azure',
    'f5f5dc': 'beige',
    'ffe4c4': 'bisque',
    '000000': 'black',
    'ffebcd': 'blanchedalmond',
    '0000ff': 'blue',
    '8a2be2': 'blueviolet',
    'a52a2a': 'brown',
    'deb887': 'burlywood',
    '5f9ea0': 'cadetblue',
    '7fff00': 'chartreuse',
    'd2691e': 'chocolate',
    'ff7f50': 'coral',
    '6495ed': 'cornflowerblue',
    'fff8dc': 'cornsilk',
    'dc143c': 'crimson',
    '00008b': 'darkblue',
    '008b8b': 'darkcyan',
    'b8860b': 'darkgoldenrod',
    'a9a9a9': 'darkgray',
    '006400': 'darkgreen',
    'bdb76b': 'darkkhaki',
    '8b008b': 'darkmagenta',
    '556b2f': 'darkolivegreen',
    'ff8c00': 'darkorange',
    '9932cc': 'darkorchid',
    '8b0000': 'darkred',
    'e9967a': 'darksalmon',
    '8fbc8f': 'darkseagreen',
    '483d8b': 'darkslateblue',
    '2f4f4f': 'darkslategray',
    '00ced1': 'darkturquoise',
    '9400d3': 'darkviolet',
    'ff1493': 'deeppink',
    '00bfff': 'deepskyblue',
    '1e90ff': 'dodgerblue',
    'b22222': 'firebrick',
    'fffaf0': 'floralwhite',
    '228b22': 'forestgreen',
    'ff00ff': 'magenta',
    'dcdcdc': 'gainsboro',
    'f8f8ff': 'ghostwhite',
    'ffd700': 'gold',
    'daa520': 'goldenrod',
    '008000': 'green',
    'adff2f': 'greenyellow',
    'f0fff0': 'honeydew',
    'ff69b4': 'hotpink',
    'cd5c5c': 'indianred',
    '4b0082': 'indigo',
    'fffff0': 'ivory',
    'f0e68c': 'khaki',
    'e6e6fa': 'lavender',
    'fff0f5': 'lavenderblush',
    '7cfc00': 'lawngreen',
    'fffacd': 'lemonchiffon',
    'add8e6': 'lightblue',
    'f08080': 'lightcoral',
    'e0ffff': 'lightcyan',
    'fafad2': 'lightgoldenrodyellow',
    'd3d3d3': 'lightgray',
    '90ee90': 'lightgreen',
    'ffb6c1': 'lightpink',
    'ffa07a': 'lightsalmon',
    '20b2aa': 'lightseagreen',
    '87cefa': 'lightskyblue',
    'b0c4de': 'lightsteelblue',
    'ffffe0': 'lightyellow',
    '00ff00': 'lime',
    '32cd32': 'limegreen',
    'faf0e6': 'linen',
    '66cdaa': 'mediumaquamarine',
    '0000cd': 'mediumblue',
    'ba55d3': 'mediumorchid',
    '9370db': 'mediumpurple',
    '3cb371': 'mediumseagreen',
    '7b68ee': 'mediumslateblue',
    '00fa9a': 'mediumspringgreen',
    '48d1cc': 'mediumturquoise',
    'c71585': 'mediumvioletred',
    'f5fffa': 'mintcream',
    'ffe4e1': 'mistyrose',
    'ffe4b5': 'moccasin',
    'ffdead': 'navajowhite',
    '000080': 'navy',
    'fdf5e6': 'oldlace',
    '6b8e23': 'olivedrab',
    'ffa500': 'orange',
    'ff4500': 'orangered',
    'da70d6': 'orchid',
    'eee8aa': 'palegoldenrod',
    '98fb98': 'palegreen',
    'afeeee': 'paleturquoise',
    'db7093': 'palevioletred',
    'ffefd5': 'papayawhip',
    'ffdab9': 'peachpuff',
    'cd853f': 'peru',
    'ffc0cb': 'pink',
    'dda0dd': 'plum',
    'b0e0e6': 'powderblue',
    'ff0000': 'red',
    'bc8f8f': 'rosybrown',
    '4169e1': 'royalblue',
    '8b4513': 'saddlebrown',
    'fa8072': 'salmon',
    'f4a460': 'sandybrown',
    '2e8b57': 'seagreen',
    'fff5ee': 'seashell',
    'a0522d': 'sienna',
    'c0c0c0': 'silver',
    '87ceeb': 'skyblue',
    '6a5acd': 'slateblue',
    'fffafa': 'snow',
    '00ff7f': 'springgreen',
    '4682b4': 'steelblue',
    'd2b48c': 'tan',
    '008080': 'teal',
    'd8bfd8': 'thistle',
    'ff6347': 'tomato',
    '40e0d0': 'turquoise',
    'ee82ee': 'violet',
    'f5deb3': 'wheat',
    'ffffff': 'white',
    'f5f5f5': 'whitesmoke',
    'ffff00': 'yellow',
    '9acd32': 'yellowgreen'
};

var colorsArr = [];
for(var key in colorMap) {
    colorsArr.push(key);
}

var filteredColors = ['rosybrown', 'darkkhaki', 'maroon', 'brown', 'chocolate', 'darkgoldenrod', 'darkred', 'darksalmon', 'goldenrod', 'indianred', 'saddlebrown', 'sienna', 'tan', 'palevioletred', 'olive'];

function generateChartColors(numberOfColors) {
    var colors = Please.make_color({
        colors_returned: (numberOfColors),
    });

    if(!colors.forEach) {
        colors = [colors];
    }

    colors = colors.filter(function(value) {
        return (filteredColors.indexOf(colorMap[closestColor(colorsArr, value.replace('#', ''))]) === -1);
    });

    if(colors.length < numberOfColors) {
        colors = colors.concat(generateChartColors(numberOfColors - colors.length));
    }

    return colors;
}

function showHideSeries (chart, data, series, columns, view, options) {
    var sel = chart.getSelection();
    // if selection length is 0, we deselected an element
    if (sel.length > 0) {
        // if row is undefined, we clicked on the legend
        if (sel[0].row == null) {
            var col = sel[0].column;
            if (columns[col] == col) {
                // hide the data series
                columns[col] = {
                    label: data.getColumnLabel(col),
                    type: data.getColumnType(col),
                    calc: function () {
                        return null;
                    }
                };

                // grey out the legend entry
                series[col - 1].color = '#CCCCCC';
            }
            else {
                // show the data series
                columns[col] = col;
                series[col - 1].color = null;
            }

            view.setColumns(columns);
            chart.draw(view, options);
        }
    }
}


function domainData(scale, callback, availability, responseTimes) {
    $.ajax({
        type: 'POST',
        url: '/ajax/domains/charts/processChartData',
        data: {scale: scale}
    })
        .done(function(response) {
            if (response.status) {
                if(response.status) {
                    callback.call(undefined, response.result, availability, responseTimes);
                }
            }
        })
        .fail(function() {

        });
}

function databaseData(scale, callback, availability, responseTimes) {
    $.ajax({
        type: 'POST',
        url: '/ajax/databases/charts/processChartData',
        data: {scale: scale}
    })
        .done(function(response) {
            if (response.status) {
                if(response.status) {
                    callback.call(undefined, response.result, availability, responseTimes);
                }
            }
        })
        .fail(function() {

        });
}

function serverData(scale, callback, availability, responseTimes) {
    $.ajax({
        type: 'POST',
        url: '/ajax/servers/charts/processChartData',
        data: {scale: scale}
    })
        .done(function(response) {
            if (response.status) {
                if(response.status) {
                    callback.call(undefined, response.result, availability, responseTimes);
                }
            }
        })
        .fail(function() {

        });
}

function drawServerCharts(result, availability, responseTimes) {
    setTimeout(function(){
        drawAvailability(availability, result, 'server');
    }, 0)

    setTimeout(function(){
        drawResponseTimes(responseTimes, result, 'server');
    }, 0)
    
    setTimeout(function(){
        drawServerHistoryUsage(result);
    }, 0);
    
    setTimeout(function(){
        drawServerHistoryIOWait(result);
    }, 0);
    
    setTimeout(function(){
        drawServerHistoryNetworkTraffic(result);
    }, 0);
    
    setTimeout(function(){
        drawServerHistoryApplicationRPM(result);
    }, 0);

    setTimeout(function(){
        drawServerHistoryTPS(result);
    }, 0);
    
    setTimeout(function(){
        drawServerHistoryApplicationResponseTime(result);
    }, 0);
    
    setTimeout(function(){
        drawServerHistoryApplicationQPM(result);
    }, 0);
    
    setTimeout(function(){
        drawServerHistoryApplicationAVGTimeCPUBound(result);
    }, 0);
}

function drawDatabaseCharts(result, availability, responseTimes) {
    setTimeout(function(){
        drawAvailability(availability, result, 'database');
    }, 0);

    setTimeout(function(){
        drawResponseTimes(responseTimes, result, 'database');
    }, 0);
    
}

function drawDomainCharts(result, availability, responseTimes) {
    setTimeout(function(){
        drawAvailability(availability, result, 'domain');
    }, 0);

    setTimeout(function(){
        drawResponseTimes(responseTimes, result, 'domain');
    }, 0);
    
}

function drawServerHistoryUsage(result) {
    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), parseFloat(value['cpu']), parseFloat(value['mem']), parseFloat(value['ds'])]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', 'CPU');
    data.addColumn('number', 'Memory');
    data.addColumn('number', 'Storage');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 1);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 2);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 3);

    var options = {
        title: 'Server Usage',
        curveType: 'function',
        colors: generateChartColors(3),
        vAxis: {
            format: '#\'%\'',
            minValue: 0.00,
            maxValue:100.00,
            baseline:0.00,
        },
        legend: {
            position: 'top',
            alignment: 'center',
        },
        hAxix: {
            'slantedTextAngle': 45,
            'slantedText': true,
        },
        explorer: {
            keepInBounds: true,
        },
    };

    drawChart('serverHistoryUsage', options, data);
}


function drawServerHistoryIOWait(result){
    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), parseFloat(value['iow']) * 100]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 1);

    var options = {
        title: 'Server IO Wait',
        curveType: 'function',
        colors: generateChartColors(1),
        legend: { position: 'none' },
        vAxis: {
            format: '#\'%\'',
            minValue: 0.00,
            maxValue:100.00,
            baseline:0.00,
        }
    };

    drawChart('serverHistoryIOWait', options, data);
}

function drawServerHistoryNetworkTraffic(result){
    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), parseFloat(value['net'])]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var options = {
        title: 'Server Network Traffic (bps)',
        curveType: 'function',
        colors: generateChartColors(1),
        legend: { position: 'none' },
        vAxis: {
            format: '#.#B',
            minValue: 0.00,
            baseline:0.00,
            viewWindow:
            {
                min:0.00
            },
        }
    };

    drawChart('serverHistoryNetworkTraffic', options, data);
}

function drawServerHistoryApplicationRPM(result){
    var dataArray = [];
    result.forEach(function(value){
        var rpm = parseFloat(value['rpm']);
        if(rpm !== 0) {
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), rpm]);
        }
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var options = {
        title: 'Application Requests Per Minute',
        curveType: 'function',
        colors: generateChartColors(1),
        legend: { position: 'none' },
        vAxis: {
            format: '#.#',
            minValue: 0.00,
            baseline:0.00,
            viewWindow:
            {
                min:0.00
            },
        }
    };

    drawChart('serverHistoryApplicationRPM', options, data);
}

function drawServerHistoryTPS(result){
    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), parseFloat(value['tps'])]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var options = {
        title: 'Server Storage IO (tps)',
        curveType: 'function',
        colors: generateChartColors(1),
        legend: { position: 'none' },
        vAxis: {
            format: '#.#',
            minValue: 0.00,
            baseline:0.00,
            viewWindow:
            {
                min:0.00
            },
        },
        hAxix: {
            'slantedTextAngle': 45,
            'slantedText': true
        },
        animation:{
            duration: 250,
            easing: 'inAndOut',
        },
        explorer: {
            keepInBounds: true
        },
    };

    drawChart('serverHistoryTPS', options, data);
}

function drawServerHistoryApplicationResponseTime(result){
    var dataArray = [];
    result.forEach(function(value){
        var avgRespTime = parseFloat(value['avgRespTime']);
        if(avgRespTime !== 0) {
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), avgRespTime]);
        }
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var options = {
        title: 'Application Response Time',
        curveType: 'function',
        colors: generateChartColors(1),
        vAxis: {
            format: '#.#ms',
            minValue: 0.00,
            baseline:0.00,
            viewWindow:
            {
                min:0.00
            },
        },
        hAxix: {
            'slantedTextAngle': 45,
            'slantedText': true
        },
        legend: {
            'position': 'none'
        },
        animation:{
            duration: 250,
            easing: 'inAndOut',
        },
        explorer: {
            keepInBounds: true
        },
        
    };

    drawChart('serverHistoryApplicationResponseTime', options, data);
}

function drawServerHistoryApplicationQPM(result){
    var dataArray = [];
    result.forEach(function(value){
        var qpm = parseFloat(value['qpm']);
        if(qpm !== 0) {
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), qpm]);
        }
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var options = {
        title: 'Application Queries Per Minute',
        curveType: 'function',
        colors: generateChartColors(1),
        vAxis: {
            format: '#.#',
            minValue: 0.00,
            baseline:0.00,
            viewWindow:
            {
                min:0.00
            },
        },
        hAxix: {
            'slantedTextAngle': 45,
            'slantedText': true
        },
        legend: {
            'position': 'none'
        },
        animation:{
            duration: 250,
            easing: 'inAndOut',
        },
        explorer: {
            keepInBounds: true
        },
        
    };

    drawChart('serverHistoryApplicationQPM', options, data);
}

function drawServerHistoryApplicationAVGTimeCPUBound(result){
    var dataArray = [];
    result.forEach(function(value){
        var avgTimeCpuBound = parseFloat(value['avgTimeCpuBound']);
        if(avgTimeCpuBound !== 0) {
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), avgTimeCpuBound]);
        }
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 1);

    var options = {
        title: 'Application Time CPU Bound Per Minute',
        curveType: 'function',
        colors: generateChartColors(1),
        vAxis: {
            format: '#\'%\'',
            minValue: 0.00,
            maxValue:100.00,
            baseline:0.00,
        },
        hAxix: {
            'slantedTextAngle': 45,
            'slantedText': true
        },
        legend: {
            'position': 'none'
        },
        animation:{
            duration: 250,
            easing: 'inAndOut',
        },
        explorer: {
            keepInBounds: true
        },
        
    };
    
    drawChart('serverHistoryApplicationAVGTimeCPUBound', options, data);
}

function drawAvailability(availability, result, type) {
    availability[type] = result;
    if(GetObjectSize(availability) === 3) {
        var dataArray = [];

        availability['server'].forEach(function(value){
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), parseFloat(value['percent']), null, null]);
        });

        availability['domain'].forEach(function(value){
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), null, parseFloat(value['percent']), null]);
        });

        availability['database'].forEach(function(value){
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), null, null, parseFloat(value['percent'])]);
        });
        
        var data = new google.visualization.DataTable();
        data.addColumn('datetime', '');
        data.addColumn('number', 'Server');
        data.addColumn('number', 'Domain');
        data.addColumn('number', 'Database');
        data.addRows(dataArray);

        var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
        dateFormatter.format(data, 0);

        var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
        percentFormatter.format(data, 1);

        var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
        percentFormatter.format(data, 2);

        var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
        percentFormatter.format(data, 3);


        var options = {
            title: 'Availability',
            curveType: 'function',
            colors: generateChartColors(3),
            vAxis: {
                format: '#\'%\'',
                minValue: 0.00,
                maxValue:100.00,
                baseline:0.00,
                viewWindowMode:'explicit',
            },
            hAxix: {
                'slantedTextAngle': 45,
                'slantedText': true
            },
            legend: {
                position: 'top',
                alignment: 'center',
            },
            animation:{
                duration: 250,
                easing: 'inAndOut',
            },
            explorer: {
                keepInBounds: true,
            },
        };

        drawChart('historyAvailability', options, data);
    }

}

function drawResponseTimes(responseTimes, result, type) {
    responseTimes[type] = result;
    if(GetObjectSize(responseTimes) === 3) {
        var dataArray = [];

        if(responseTimes['server']) {
            responseTimes['server'].forEach(function(value){
                var date = new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1)));
                dataArray.push([date, parseFloat(value['responseTime']) * 1000, null, null]);
            });
        }

        if(responseTimes['domain']) {
            responseTimes['domain'].forEach(function(value){
                var date = new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1)));
                dataArray.push([date, null, parseFloat(value['responseTime']) * 1000, null]);
            });
        }

        if(responseTimes['database']) {
            responseTimes['database'].forEach(function(value){
                var date = new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1)));
                dataArray.push([date, null, null, parseFloat(value['responseTime']) * 1000]);
            });
        }
        
        var data = new google.visualization.DataTable();
        data.addColumn('datetime', '');
        data.addColumn('number', 'Server');
        data.addColumn('number', 'Domain');
        data.addColumn('number', 'Database');
        data.addRows(dataArray);

        var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
        dateFormatter.format(data, 0);

        var options = {
            title: 'Response Time',
            curveType: 'function',
            colors: generateChartColors(3),
            vAxis: {
                format: '#.#ms',
                minValue: 0.00,
                baseline:0.00,
            },
            hAxix: {
                'slantedTextAngle': 45,
                'slantedText': true,
            },
            legend: {
                position: 'top',
                alignment: 'center',
            },
            explorer: {
                keepInBounds: true,
            },
        };
        
        drawChart('historyResponseTime', options, data);
    }
}

function drawChart(id, options, data, columns, series) {
    var chart = new google.visualization.AreaChart(document.getElementById(id));
    
    if(!columns) {
        // create columns array
        var columns = [];

        // display these data series by default
        var defaultSeries = [1, 2, 3];
        var series = {};
        for (var i = 0; i < data.getNumberOfColumns(); i++) {
            if (i == 0 || defaultSeries.indexOf(i) > -1) {
                // if the column is the domain column or in the default list, display the series
                columns.push(i);
            }
            else {
                // otherwise, hide it
                columns[i] = {
                    label: data.getColumnLabel(i),
                    type: data.getColumnType(i),
                    calc: function () {
                        return null;
                    }
                };
            }
            if (i > 0) {
                // set the default series option
                series[i - 1] = {};
                if (defaultSeries.indexOf(i) == -1) {
                    // backup the default color (if set)
                    if (typeof(series[i - 1].color) !== 'undefined') {
                        series[i - 1].backupColor = series[i - 1].color;
                    }
                    series[i - 1].color = '#CCCCCC';
                }
            }
        }

        options.series = series;
    }
        
    // create a view with the default columns
    var view = new google.visualization.DataView(data);
    view.setColumns(columns);
    google.visualization.events.addListener(chart, 'select', function(){
        showHideSeries(chart, data, series, columns, view, options);
    });

    chart.draw(view, options);

    makeChartResponsive(id, options, data, columns, series);
}

function makeChartResponsive(id, options, data, columns, series) {
    $(window).on('resizeStart', function() {
        $('#' + id).html('<div class="spinner"><div class="rect1"></div><div class="rect2"></div><div class="rect3"></div><div class="rect4"></div><div class="rect5"></div></div>');
    });
    
    $(window).on('resizeEnd', function() {
        if(!this.drawChartTimeout) {
            this.drawChartTimeout = {};
        }
        
        if(this.drawChartTimeout[id]) clearTimeout(this.drawChartTimeout[id]);
        this.drawChartTimeout[id] = setTimeout(function() {
            drawChart(id, options, data, columns, series);
        }, 500);
    });
}

function drawChartsTrigger() {
    var availability = {};
    var responseTimes = {};
    var scale = $('#timeScale').val();
    $('.chart').html('<div class="spinner"><div class="rect1"></div><div class="rect2"></div><div class="rect3"></div><div class="rect4"></div><div class="rect5"></div></div>');
    setTimeout(function(){
        domainData(scale, drawDomainCharts, availability, responseTimes);
    },0);
    setTimeout(function(){
        databaseData(scale, drawDatabaseCharts, availability, responseTimes);
    },0);
    setTimeout(function(){
        serverData(scale, drawServerCharts, availability, responseTimes);
    },0);
}

$(document).ready(function (){
    $(document).on('chartDraw', function(){
        drawChartsTrigger();
    });
    
    $(document).on('change', '#timeScale', function(){
        drawChartsTrigger();
    });

    if (screenfull.enabled) {
        document.addEventListener(screenfull.raw.fullscreenchange, function () {
            $(this).trigger('resizeStart');
            if(screenfull.isFullscreen) {
                $(screenfull.element).css('height', '100%').addClass('fullscreen');
            } else {
                $('.fullscreen').each(function(index, value) {
                    var $this = $(value);
                    $this.css('height', 200);
                    $this.removeClass('fullscreen');
                });
            }
            $(this).trigger('resizeEnd');
        });
    }
    
    $(document).on('dblclick', '.chart', function(){
        if (screenfull.enabled) {
            screenfull.toggle(this);
        }
    });
    
});