
/*
 * clothes counts
 */

var cur_max_wear;

$('#btn-clothes-toggle').click(function(){
    $('.cnt-owned').toggle()
    $('.cnt-free').toggle()
})

/*
$('.clothes-counts a').click(function(){
    $.ajax({
        url: this.href + "&free_max_wear="+cur_max_wear,
        success: function(html){
            $('#clothes-tbl-place').html(html)
            $('table.sortable','#clothes-tbl-place').each(function(_,tbl){
                sorttable.makeSortable(tbl);
            })
        }
    });
    return false;
})
*/

if($('h2.cnt-free')[0]){ // only if element exists
    function cf_update(){
        var new_max_wear = $('h2.cnt-free select').val();
        if( new_max_wear != cur_max_wear ){
            window.location.href = "?free_max_wear=" + new_max_wear;
        }
    }

    $('h2.cnt-free').append(
        "<div id=max-wear-sel>" +
        "<label>max wear: </label>" +
        "<select>" +
            "<option value=0>(item)" +
            "<option value=1>x(item)x" +
            "<option value=2>X(item)X" +
            "<option value=3>XX(item)XX" +
        "</select>" +
        "</div>"
    );
    $('h2.cnt-free label').click(function(){
        var select = $('h2.cnt-free select')
        if(select.is(":visible")){
            cf_update();
        } else {
            $('h2.cnt-free label').text("max wear: ");
        }
    });
    $('h2.cnt-free select').blur(cf_update).change(cf_update);

    if(window.location.href.match(/free_max_wear=(\d+)/)){
        cur_max_wear = RegExp.$1;
        $('h2.cnt-free select').val(cur_max_wear);
    }
}

/*
 * crosshair
 */

$('.crosshair').attr('title', 'Center screen on this unit').click(function(){
    var parent = $(this).closest('tr')
    var id = parent.attr('id')
    if(!id || id.indexOf('unit_') != 0){
        alert("cannot find unit id parent")
        return
    }

    id = parseInt(id.replace("unit_",""))
    $(this).parent().effect('highlight')

    $.ajax({ url: "/units?id=" + id + "&center=1" });
})


/*
 * dwarf info page
 */

if($('.thoughts')[0]){ // only if element exists
    $('.thoughts').html(
        "<p>" + $('.thoughts').html().
        replace(/\[B\]/g,'').
        split("[P]").
        join("</p><p>").
        concat("</p>").
        replace(/\[(C.+?)\](.+?)\[/g, function(x,y,z){
            return "<span class='color-" + y.replace(/[^\d]/g,'') + "'>" + z + "</span>["
        }).
        replace(/\[(C.+?)\](.+?)</g, function(x,y,z){
            return "<span class='color-" + y.replace(/[^\d]/g,'') + "'>" + z + "</span><"
        })
    );
}

/*
 * autorefresh
 */

var ar_timeout_id;

function ar_update(nSeconds){
    clearTimeout(ar_timeout_id);
    var select = $('.autorefresh select')
    if(typeof(nSeconds) != "number") nSeconds = parseInt(select.val());
    select.hide()
    if(!isNaN(nSeconds) && nSeconds > 1){
        $('.autorefresh label').text("autorefresh: " + nSeconds + "s");
        window.location.hash = "refresh=" + nSeconds;
        ar_timeout_id = setTimeout(function(){ window.location.reload() }, nSeconds*1000);
    } else {
        $('.autorefresh label').text("autorefresh: no");
    }
}

$(function(){
    $('body').append(
        "<div class=autorefresh>" +
        "<label>autorefresh: no</label>" +
        "<select> <option>no <option>5s <option>10s <option>30s <option>60s </select>" +
        "</div>"
    );
    $('.autorefresh label').click(function(){
        var select = $('.autorefresh select')
        if(select.is(":visible")){
            ar_update();
        } else {
            clearTimeout(ar_timeout_id);
            $('.autorefresh label').text("autorefresh: ");
            select.show();
        }
    });
    $('.autorefresh select').blur(ar_update).change(ar_update);

    if(window.location.href.match(/refresh=(\d+)/)){
        var nSeconds = parseInt(RegExp.$1);
        if(!isNaN(nSeconds) && nSeconds > 1){
            $('.autorefresh select').val(nSeconds + "s");
            ar_update(nSeconds)
        }
    }
});
