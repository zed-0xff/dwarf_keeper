
// http://stackoverflow.com/questions/920236/jquery-detect-if-selector-returns-null
$.fn.exists = $.fn.exist = function () {
    return this.length !== 0;
}

// http://stackoverflow.com/a/2880929/286595
var $url_params = {};
(function () {
    var e,
        a = /\+/g,  // Regex for replacing addition symbol with a space
        r = /([^&=]+)=?([^&]*)/g,
        d = function (s) { return decodeURIComponent(s.replace(a, " ")); },
        q = window.location.search.substring(1);

    while (e = r.exec(q))
       $url_params[d(e[1])] = d(e[2]);
})();

/******************************************************************************
 * clothes counts
 ******************************************************************************/

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

/******************************************************************************
 * crosshair
 ******************************************************************************/

$('.crosshair').attr('title', 'Center screen on this unit').click(function(){
    var parent = $(this).closest('tr')
    var id = parent.attr('id')
    if(!id || id.indexOf('unit_') != 0){
        alert("cannot find unit id parent")
        return
    }

    id = parseInt(id.replace("unit_",""))
    $(this).parent().effect('highlight')

    $.ajax({ url: "/screen?unit_id=" + id });
})

/******************************************************************************
 * unit info
 ******************************************************************************/

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

/******************************************************************************
 * autorefresh
 ******************************************************************************/

var ar_timeout_id;

function ar_update(nSeconds){
    clearTimeout(ar_timeout_id);
    var select = $('.autorefresh select')
    if(typeof(nSeconds) != "number") nSeconds = parseInt(select.val());
    select.hide()
    if(!isNaN(nSeconds) && nSeconds > 0){
        $('.autorefresh label').text("autorefresh: " + nSeconds + "s");
        window.location.hash = "refresh=" + nSeconds;
        ar_timeout_id = setTimeout(function(){ window.location.reload() }, nSeconds*1000);
    } else {
        $('.autorefresh label').text("autorefresh: no");
    }
}

$(function(){
    if( !$("div.error").exists() ){
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

        if(window.location.href.match(/refresh=([\d.]+)/)){
            var nSeconds = parseFloat(RegExp.$1);
            if(!isNaN(nSeconds) && nSeconds > 0){
                $('.autorefresh select').val(nSeconds + "s");
                ar_update(nSeconds)
            }
        }
    }
});

/******************************************************************************
 * trade
 ******************************************************************************/

if( $('div#trade').exists() ){
    $('#trade tbody.category').click(function(){
        var id = this.id
        if(!id) return
        id = id.substr(3) // "cat12" => "12"
        $('#tb'+id, $(this).closest('table')).toggle();
    })

    $('.tristate').click(function(){
        var ts = $(this)
        var type_id = ts.closest('tbody').attr('id').replace("cat","")
        var table = ts.closest('table')
        var checkboxes = $("#tb"+type_id+" input", table)
        var state;

        if( ts.is(".checked, .intermediate") ){
            state = 0;
            ts.removeClass("checked intermediate")
            // remove all checks from children
            checkboxes.attr("checked", false)
        } else {
            state = 1;
            ts.addClass("checked")
            // check all children
            checkboxes.attr("checked", true)
        }

        $.ajax({ 
            url:      "/trade?type_id=" + type_id +
                      "&state=" + state + 
                      "&side=" + (table.hasClass("side_left") ? "left" : "right"),
            dataType: 'json',
            success:  trade_ajax_success,
            context:  table
        });
        return false;
    })

    function update_tristate_state(ts){
        var ts = $(ts)
        if( !ts.exists() ) return; // drinks and liquids have separate categories, but are not sold w/o containers

        var id = ts.closest('tbody').attr('id').replace("cat","tb")
        var checkboxes = $("#"+id+" input", ts.closest('table'))
        var has_checked = checkboxes.filter(":checked").exists()
        var has_unchecked = checkboxes.not(":checked").exists()
    
        if( has_checked && has_unchecked ){
            ts.removeClass("checked").addClass("intermediate")
        } else if( has_checked ){
            ts.removeClass("intermediate").addClass("checked")
        } else if( has_unchecked ){
            ts.removeClass("checked intermediate")
        }
    }

    function trade_ajax_success(data){
        // uncheck checkboxes
        $(data.ids.map(function(id){ return "#i" + id }).join(",")).attr('checked',false);
        // update tristates
        forEach(data.types, function(cat_id){
            var tristate = $("#cat" + cat_id + " .tristate", this);
            update_tristate_state(tristate);
        }, this);
    }

    // for checkboxes shift-click
    var last_checkbox = null;

    // checkboxes
    $('#trade input').click(function(ev, shift_flag){
        var id = $(this).closest('tbody').attr('id').replace("tb","cat")
        var table = $(this).closest('table')
        var tristate = $("#"+id+" .tristate", table)
        var from_tr = (shift_flag !== undefined)
        var checkboxes;

        var state = this.checked
        if(from_tr) state = !state;

        if (last_checkbox != null && ev && (ev.shiftKey || ev.metaKey || shift_flag)) {
            // checkboxes shift-click
            // cannot cache all_checkboxes b/c their order is changed when changing table sort
            var all_checkboxes = $('#trade input');
            checkboxes = all_checkboxes.slice(
                Math.min(all_checkboxes.index(last_checkbox), all_checkboxes.index(ev.target)),
                Math.max(all_checkboxes.index(last_checkbox), all_checkboxes.index(ev.target)) + 1
            ).attr( "checked", state );
        } else {
            checkboxes = $(this);
        }

        var url = "/trade?state=" + (state ? 1 : 0)
        url += checkboxes.map(function(){ return "&id=" + this.id.replace('i','') }).get().join('')

        last_checkbox = this;

        // works wrong w/o timeout when called from tr click() handler
        setTimeout(function(){ update_tristate_state(tristate) }, 100);

        $.ajax({ 
            url:      url,
            dataType: 'json',
            success:  trade_ajax_success,
            context:  table
        });

        return !from_tr || !shift_flag;
    })

    // update tristates on load
    forEach( $('.tristate'), function(ts){
        update_tristate_state(ts)
    })

    // click on item row
    $('td.name').click(function(ev){
        if( ev.target.tagName == "TD" ){
            $('input',this).trigger("click", ev.shiftKey || ev.metaKey )
        }
    })
}

/******************************************************************************
 * units list
 ******************************************************************************/

if( $("#units").exists() ){
    // highlight grepped string
    if( $url_params['grep'] ){
        $("input[name=grep]").val( $url_params['grep'] )
        var encoded_grep_param = encodeURIComponent( $url_params['grep'] )
        $('#units table a').each(function(){
            // only find links to unit info page
            if( this.href && this.href.match(/[&?]id=\d/) ){
                this.href += "&hl=" + encoded_grep_param
            }
        })
    }
}

/******************************************************************************
 * coords links
 ******************************************************************************/

$('a.coords').click(function(){
    $.ajax({ 
        url: this.href,
    success: function(){ $(this).effect('highlight', 1000) },
    context: this
    });
    return false;
})

/******************************************************************************
 * live display
 ******************************************************************************/

if($('pre#live').exist()){

    var key_ts = 0;
    var last_keypress = null;

    function key_js2sdl(js_key){
        switch(js_key){
            case  27: // SDLK_ESCAPE
                return js_key;

            case  33: return 280; // SDLK_PAGEUP
            case  34: return 281; // SDLK_PAGEDOWN
            case  37: return 276; // SDLK_LEFT
            case  38: return 273; // SDLK_UP
            case  39: return 275; // SDLK_RIGHT
            case  40: return 274; // SDLK_DOWN

            // F1-F12
            case 112: case 113: case 114: case 115: case 116: case 117:
            case 118: case 119: case 120: case 121: case 122: case 123:
                return parseInt(js_key)+170;
        }
        return 0;
    }

    function live_key(ev){
        ev.stopPropagation();
        ev.preventDefault();

        if( ev.type == 'keypress' ){
            last_keypress = ev;
            return;
        }

        var key, ukey = null;

        // ev.type == 'keyup'
        if( last_keypress && ev.timeStamp - last_keypress.timeStamp < 250 ){
            ukey = last_keypress.keyCode
            if( 
                (ev.altKey  && last_keypress.altKey  ) ||
                (ev.ctrlKey && last_keypress.ctrlKey )
            ){
                // not sure why
                key = ev.keyCode;
                if( key >= 65 && key <= 90 ) key += 32; // A..Z => a..z
            } else {
                key = last_keypress.keyCode
            }
        } else {
            key = key_js2sdl(ev.which)
        }
        last_keypress = null

        if(!key){
            // unknown key, ignore single modifier keys
            if( ev.which != 16 && ev.which !=17 ) console.log(ev.which)
            return
        }

        var url = "?key="   + key + 
                  "&shift=" + (ev.shiftKey?1:0) +
                  "&alt="   + (ev.altKey?1:0)   +
                  "&ctrl="  + (ev.ctrlKey?1:0)  +
                  "&meta="  + (ev.metaKey?1:0);

        if( ukey ) url += "&ukey=" + ukey; // unicode key

        $.ajax(url);
        console.log(url);
    }

    $(document.documentElement).keypress(live_key).keyup(live_key)

    $(function(){
        // Wrap this function in a closure so we don't pollute the namespace
        (function worker(){
          $.ajax({
            url: '?hash='+$('#live-hash').text(), 
            success: function(data) {
                if( data == "NOT_MODIFIED" ){
                    setTimeout(worker, 100);
                } else {
                    $('pre#live').html(data);
                    setTimeout(worker, 10);
                }
            },
            error: function() {
              setTimeout(worker, 2000)
            }
          })
        })();
    });
}
