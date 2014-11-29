'use strict';

function loadTemplate(id) {
    window.tables = [];
    var $template = $('#template');
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
                    response.result.forEach(function(value){
                        var labelRow = '<div>';
                        var columnCount = value['columns'].length + 1;
                        labelRow += '<div class="col-sm-1">';

                        labelRow += '<p>' + value['name'] + '</p>';

                        labelRow += '</div>';

                        value['columns'].forEach(function(value) {
                            labelRow += '<div class="col-sm-1">';

                            labelRow += '<p>' + value['name'] + '</p>';

                            labelRow += '</div>';
                        });

                        labelRow += '</div><div class="clearfix"></div>';
                        //$template.append(labelRow);                        
                    });
                }, 1000);
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
    
}

$(document).ready(function(){
    $(document).on('change', '#templateSelect', function(){
        loadTemplate($(this).val());
    });
});