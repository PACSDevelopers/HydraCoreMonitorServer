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
                    response.result.forEach(function(value){
                        var labelRow = '<li class="list-group-item col-xs-12 col-md-6 col-lg-3" id="tableRow' +  value['id'] + '">';
                        labelRow += '<div class="pull-left text-left">' + value['name'] + '</div>';
                        labelRow += '<div class="pull-right text-right"><input type="checkbox" data-toggle="toggle" class="form-checkbox checkbox-inline tableRowCheckbox" data-id="'+  value['id'] + '" id="tableRowCheckbox' +  value['id'] + '" /></div>';
                        labelRow += '<div class="cleafix"></div>';
                        labelRow += '</li>';
                        $template.append(labelRow);
                        $('.tableRowCheckbox').bootstrapToggle();
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