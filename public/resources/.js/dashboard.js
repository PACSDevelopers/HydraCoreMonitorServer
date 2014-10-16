'use strict';

function domainData(callback) {
    $.ajax({
        type: "POST",
        url: '/ajax/domains/charts/processDayChart',
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.status) {
                    callback.call(undefined, response.result);
                }
            }
        })
        .fail(function() {

        });
}

function databaseData(callback) {
    $.ajax({
        type: "POST",
        url: '/ajax/databases/charts/processDayChart',
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.status) {
                    callback.call(undefined, response.result);
                }
            }
        })
        .fail(function() {

        });
}

function serverData(callback) {
    $.ajax({
        type: "POST",
        url: '/ajax/servers/charts/processDayChart',
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.status) {
                    callback.call(undefined, response.result);
                }
            }
        })
        .fail(function() {

        });
}

function drawServerCharts(result) {
    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(value['dateCreated']), parseFloat(value['percent'])]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/mm/yyyy hh:mm:ss aa'});
    dateFormatter.format(data, 0);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 1);

    var options = {
        title: 'Server Availability',
        curveType: 'function',
        legend: { position: 'none' },
        hAxis:{
            baselineColor: '#fff',
            gridlineColor: '#fff',
            textPosition: 'none'
        },
        vAxis: {
            format: '#\'%\'',
            minValue: 0.00,
            maxValue:100.00,
            baseline:0.00,
            viewWindowMode:'explicit',
            viewWindow:
            {
                max:100.00,
                min:0.00
            },
            gridlines:{count:3}
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('serverHistoryAvailability'));

    chart.draw(data, options);

    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(value['dateCreated']), parseFloat(value['responseTime']) * 1000]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/mm/yyyy hh:mm:ss aa'});
    dateFormatter.format(data, 0);

    var options = {
        title: 'Server Response Time',
        curveType: 'function',
        legend: { position: 'none' },
        hAxis:{
            baselineColor: '#fff',
            gridlineColor: '#fff',
            textPosition: 'none'
        },
        vAxis: {
            format: '#.#ms',
            minValue: 0.00,
            baseline:0.00,
            viewWindowMode:'explicit',
            viewWindow:
            {
                min:0.00
            },
            gridlines:{count:3}
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('serverHistoryResponseTime'));

    chart.draw(data, options);
}

function drawDatabaseCharts(result) {
    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(value['dateCreated']), parseFloat(value['percent'])]);
    });
    
    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/mm/yyyy hh:mm:ss aa'});
    dateFormatter.format(data, 0);

    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 1);


    var options = {
        title: 'Database Availability',
        curveType: 'function',
        legend: { position: 'none' },
        hAxis:{
            baselineColor: '#fff',
            gridlineColor: '#fff',
            textPosition: 'none'
        },
        vAxis: {
            format: '#\'%\'',
            minValue: 0.00,
            maxValue:100.00,
            baseline:0.00,
            viewWindowMode:'explicit',
            viewWindow:
            {
                max:100.00,
                min:0.00
            },
            gridlines:{count:3}
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('databaseHistoryAvailability'));

    chart.draw(data, options);

    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(value['dateCreated']), parseFloat(value['responseTime']) * 1000]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/mm/yyyy hh:mm:ss aa'});
    dateFormatter.format(data, 0);

    var options = {
        title: 'Database Response Time',
        curveType: 'function',
        legend: { position: 'none' },
        hAxis:{
            baselineColor: '#fff',
            gridlineColor: '#fff',
            textPosition: 'none'
        },
        vAxis: {
            format: '#.#ms',
            minValue: 0.00,
            baseline:0.00,
            viewWindowMode:'explicit',
            viewWindow:
            {
                min:0.00
            },
            gridlines:{count:3}
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('databaseHistoryResponseTime'));

    chart.draw(data, options);
}

function drawDomainCharts(result) {
    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(value['dateCreated']), parseFloat(value['percent'])]);
    });
    
    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/mm/yyyy hh:mm:ss aa'});
    dateFormatter.format(data, 0);
    
    var percentFormatter = new google.visualization.NumberFormat({pattern: '#\'%\''});
    percentFormatter.format(data, 1);

    var options = {
        title: 'Domain Availability',
        curveType: 'function',
        legend: { position: 'none' },
        hAxis:{
            baselineColor: '#fff',
            gridlineColor: '#fff',
            textPosition: 'none'
        },
        vAxis: {
            format: '#\'%\'',
            minValue: 0.00,
            maxValue:100.00,
            baseline:0.00,
            viewWindowMode:'explicit',
            viewWindow:
            {
                max:100.00,
                min:0.00
            },
            gridlines:{count:3}
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('domainHistoryAvailability'));

    chart.draw(data, options);

    var dataArray = [];
    result.forEach(function(value){
        dataArray.push([new Date(value['dateCreated']), parseFloat(value['responseTime']) * 1000]);
    });

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', '');
    data.addColumn('number', '');
    data.addRows(dataArray);

    var dateFormatter = new google.visualization.DateFormat({pattern: 'dd/mm/yyyy hh:mm:ss aa'});
    dateFormatter.format(data, 0);
    
    var options = {
        title: 'Domain Response Time',
        curveType: 'function',
        legend: { position: 'none' },
        hAxis:{
            baselineColor: '#fff',
            gridlineColor: '#fff',
            textPosition: 'none'
        },
        vAxis: {
            format: '#.#ms',
            minValue: 0.00,
            baseline:0.00,
            viewWindowMode:'explicit',
            viewWindow:
            {
                min:0.00
            },
            gridlines:{count:3}
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('domainHistoryResponseTime'));

    chart.draw(data, options);
}

$(document).ready(function (){
    
    $(document).on('chartDraw', function(){
        domainData(drawDomainCharts);
        databaseData(drawDatabaseCharts);
        serverData(drawServerCharts);
    });
    
});