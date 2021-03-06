/* vim: set ts=8 et sw=4 sta ai cin: */
/*
 * @author Paulo Pizarro  <paulo.pizarro@gmail.com>
 * @author Tiago Katcipis <tiagokatcipis@gmail.com>
 *
 * This file is part of Luasofia.
 *
 * Luasofia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Luasofia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Luasofia.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "luasofia.h"
#include "utils/luasofia_tags.h"
#include "utils/luasofia_const.h"
#include "utils/luasofia_proxy.h"

#include <sofia-sip/sdp.h>
#include <sofia-sip/sdp_tag.h>

#include "luasofia_sdp_session_private.h"
#include "luasofia_sdp_attribute_private.h"
#include "luasofia_sdp_bandwidth_private.h"
#include "luasofia_sdp_connection_private.h"
#include "luasofia_sdp_key_private.h"
#include "luasofia_sdp_media_private.h"
#include "luasofia_sdp_rtpmap_private.h"
#include "luasofia_sdp_list_private.h"
#include "luasofia_sdp_origin_private.h"
#include "luasofia_sdp_repeat_private.h"
#include "luasofia_sdp_time_private.h"
#include "luasofia_sdp_parser.h"


static const luaL_Reg sdp_lib[] = {
    {"parse"                , luasofia_sdp_parser_parse},
    {"attribute_find"       , luasofia_sdp_attribute_find},
    {"get_proxy_session"    , luasofia_sdp_get_proxy_session},
    {"get_proxy_attribute"  , luasofia_sdp_get_proxy_attribute},
    {"get_proxy_bandwidth"  , luasofia_sdp_get_proxy_bandwidth},
    {"get_proxy_connection" , luasofia_sdp_get_proxy_connection},
    {"get_proxy_key"        , luasofia_sdp_get_proxy_key},
    {"get_proxy_media"      , luasofia_sdp_get_proxy_media},
    {"get_proxy_rtpmap"     , luasofia_sdp_get_proxy_rtpmap},
    {"get_proxy_list"       , luasofia_sdp_get_proxy_list},
    {"get_proxy_origin"     , luasofia_sdp_get_proxy_origin},
    {"get_proxy_repeat"     , luasofia_sdp_get_proxy_repeat},
    {"get_proxy_time"       , luasofia_sdp_get_proxy_time},
    {NULL, NULL}
};

static const luasofia_tag_reg_t sdp_tags[] = {
    {"SDPTAG_ANY", sdptag_any },
    {"SDPTAG_SESSION", sdptag_session },
    { NULL, NULL}
};

static const luasofia_reg_const_t sdp_constants[] = {
    {"sdp_net_x",  sdp_net_x},
    {"sdp_net_in", sdp_net_in},
    {"sdp_addr_x", sdp_addr_x},
    {"sdp_addr_ip4", sdp_addr_ip4},
    {"sdp_addr_ip6", sdp_addr_ip6},
    {"sdp_bw_x", sdp_bw_x},
    {"sdp_bw_ct", sdp_bw_ct},
    {"sdp_bw_as", sdp_bw_as},
    {"sdp_media_x", sdp_media_x},
    {"sdp_media_any", sdp_media_any},
    {"sdp_media_audio", sdp_media_audio},
    {"sdp_media_video", sdp_media_video},
    {"sdp_media_application", sdp_media_application},
    {"sdp_media_data", sdp_media_data},
    {"sdp_media_control", sdp_media_control},
    {"sdp_media_message", sdp_media_message},
    {"sdp_media_image", sdp_media_image},
    {"sdp_media_red", sdp_media_red},
    {"sdp_proto_x", sdp_proto_x},
    {"sdp_proto_tcp", sdp_proto_tcp},
    {"sdp_proto_udp", sdp_proto_udp},
    {"sdp_proto_rtp", sdp_proto_rtp},
    {"sdp_proto_srtp", sdp_proto_srtp},
    {"sdp_proto_udptl", sdp_proto_udptl},
    {"sdp_proto_tls", sdp_proto_tls},
    {"sdp_proto_any", sdp_proto_any},
    {"sdp_inactive", sdp_inactive},
    {"sdp_sendonly", sdp_sendonly},
    {"sdp_recvonly", sdp_recvonly},
    {"sdp_sendrecv", sdp_sendrecv},
    {"sdp_f_strict", sdp_f_strict},
    {"sdp_f_anynet", sdp_f_anynet},
    {"sdp_f_realloc",sdp_f_realloc},
    {"sdp_f_all_rtpmaps", sdp_f_all_rtpmaps},
    {"sdp_f_print_prefix",sdp_f_print_prefix},
    {"sdp_f_mode_0000", sdp_f_mode_0000},
    {"sdp_f_insane", sdp_f_insane},
    {"sdp_f_c_missing", sdp_f_c_missing},
    {"sdp_f_config", sdp_f_config},
    {"sdp_f_mode_manual", sdp_f_mode_manual},
    {"sdp_f_mode_always", sdp_f_mode_always},
    {NULL, 0 }
};

int luaopen_sofia_sdp(lua_State *L)
{
    luaopen_sofia(L);

    /* sofia[sdp] = table */
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, -3, "sdp");
    luaL_register(L, NULL, sdp_lib);

    luasofia_tags_register_tags(L, sdp_tags);

    luasofia_proxy_register_info_table(L, SDP_SESSION_TABLE_NAME,   sdp_session_info);    
    luasofia_proxy_register_info_table(L, SDP_ATTRIBUTE_TABLE_NAME, sdp_attribute_info);    
    luasofia_proxy_register_info_table(L, SDP_BANDWIDTH_TABLE_NAME, sdp_bandwidth_info);
    luasofia_proxy_register_info_table(L, SDP_CONNECTION_TABLE_NAME,sdp_connection_info);
    luasofia_proxy_register_info_table(L, SDP_KEY_TABLE_NAME,       sdp_key_info);
    luasofia_proxy_register_info_table(L, SDP_MEDIA_TABLE_NAME,     sdp_media_info);
    luasofia_proxy_register_info_table(L, SDP_RTPMAP_TABLE_NAME,    sdp_rtpmap_info);
    luasofia_proxy_register_info_table(L, SDP_LIST_TABLE_NAME,      sdp_list_info);
    luasofia_proxy_register_info_table(L, SDP_REPEAT_TABLE_NAME,    sdp_repeat_info);
    luasofia_proxy_register_info_table(L, SDP_ORIGIN_TABLE_NAME,    sdp_origin_info);
    luasofia_proxy_register_info_table(L, SDP_TIME_TABLE_NAME,      sdp_time_info);

    luasofia_const_register_constants(L, sdp_constants);
    luasofia_sdp_parser_register_meta(L);

    return 1;
}

