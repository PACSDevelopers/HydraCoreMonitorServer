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
        url: '/ajax/domains/charts/processChartDataSingle',
        data: {scale: scale, domainID: $('#domainID').val()}
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

function drawDomainCharts(result, availability, responseTimes) {
    setTimeout(function(){
        drawAvailability(availability, result, 'domain');
    }, 0);

    setTimeout(function(){
        drawResponseTimes(responseTimes, result, 'domain');
    }, 0);
    
}

function drawAvailability(availability, result, type) {
    availability[type] = result;
    var dataArray = [];

    availability['domain'].forEach(function(value){
        var status = 0;
        if(value['status'] == 200) {
            status = 100;    
        }
        dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), status]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);
    console.log(dataArray);
    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/MM/yyyy hh:mm:ss.SSSS aa'});
    dateFormatter.format(data, 0);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 1);


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
            position: 'none',
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

function drawResponseTimes(responseTimes, result, type) {
    responseTimes[type] = result;
    
    var dataArray = [];

    if(responseTimes['domain']) {
        responseTimes['domain'].forEach(function(value){
            dataArray.push([new Date(parseInt(value['dateCreated'].replace('.', '').slice(0,-1))), parseFloat(value['responseTime']) * 1000]);
        });
    }
    
    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
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
            position: 'none',
        },
        explorer: {
            keepInBounds: true,
        },
    };

    drawChart('historyResponseTime', options, data);
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