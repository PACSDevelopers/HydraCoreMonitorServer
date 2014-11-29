'use strict';

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
            if (typeof(response.status) != 'undefined') {
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