'use strict';

function loadTemplate(id) {
    window.tables = [];
    var $template = $('#tableRowList');
    $template.html('');

    var $alertBox = $('#alertBox');
    var data = {'templateID': id};

    $alertBox.html(bootstrapAlert('info', 'Downloading template.')).slideDown();

    $.ajax({
        type: "POST",
        url: '/ajax/templates/table/loadAll',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                $alertBox.html(bootstrapAlert('info', 'Loading template.'));

                setTimeout(function(){
                    console.log(response);
                    window.selectedTables = [];
                    response.result.forEach(function(value){
                        var labelRow = '<li class="list-group-item col-xs-12 col-md-6 col-lg-3" id="tableRow' +  value['id'] + '">';
                        labelRow += '<div class="pull-left" style="max-width: 170px; overflow-wrap: break-word;"><p>' + value['alias'] + '</p></div>';
                        labelRow += '<div class="pull-right text-right"><input type="checkbox" data-toggle="toggle" class="form-checkbox checkbox-inline tableRowCheckbox" data-id="'+  value['id'] + '" id="tableRowCheckbox' +  value['id'] + '" /></div>';
                        labelRow += '<div class="cleafix"></div>';
                        labelRow += '</li>';
                        $template.append(labelRow);
                    });

                    $('.tableRowCheckbox').bootstrapToggle();
                    $alertBox.html(bootstrapAlert('success', 'Loaded template.'));

                }, 1000);
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });

}

function runExport() {
    var $alertBox = $('#alertBox');
    var data = {'templateID': $('#templateSelect').val(), 'databaseID': $('#databaseID').val(), 'selectedTables': window.selectedTables};

    $alertBox.html(bootstrapAlert('info', 'Scheduling export.')).slideDown();

    $.ajax({
        type: "POST",
        url: '/ajax/exports/export/schedule',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                $alertBox.html(bootstrapAlert('success', 'Scheduled export.'));
            } else {
                $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });

}

function bindEvents() {
    $(document).on('change', '.tableRowCheckbox', function(){
        var $this = $(this);
        var value = $this.is(':checked');
        var id = $this.attr('data-id');
        if(value) {
            window.selectedTables.push(id);
        } else {
            var index = window.selectedTables.indexOf(id);
            if (index > -1) {
                window.selectedTables.splice(index, 1);
            }
        }
    });
}

$(document).ready(function(){
    bindEvents();
    $(document).on('change', '#templateSelect', function(){
        loadTemplate($(this).val());
    });
});