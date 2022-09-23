{
    "variables": {
        "openssl_fips" : "0"
    },
  'targets': [
    {
      'target_name': 'slp',
      'defines': [
            'HAVE_CONFIG_H',
            'ENABLE_ASYNC_API',
      ],
      'include_dirs' : [
          "<!(node -e \"require('nan')\")",
          "src",
          "src/openslp/openslp/common",
          "src/openslp/openslp/libslp"
      ],
      'sources': [
        'src/openslp/openslp/common/slp_spi.c',
#        'src/openslp/openslp/common/slp_predicate.c',
        'src/openslp/openslp/common/slp_network.c',
        'src/openslp/openslp/common/slp_compare.c',
        'src/openslp/openslp/common/slp_dhcp.c',
        'src/openslp/openslp/common/slp_auth.c',
        'src/openslp/openslp/common/slp_debug.c',
        'src/openslp/openslp/common/slp_thread.c',
        'src/openslp/openslp/common/slp_buffer.c',
        'src/openslp/openslp/common/slp_xcast.c',
        'src/openslp/openslp/common/slp_property.c',
        'src/openslp/openslp/common/slp_v2message.c',
        'src/openslp/openslp/common/slp_atomic.c',
        'src/openslp/openslp/common/slp_database.c',
        'src/openslp/openslp/common/slp_parse.c',
        'src/openslp/openslp/common/slp_linkedlist.c',
        'src/openslp/openslp/common/slp_utf8.c',
        'src/openslp/openslp/common/slp_pid.c',
        'src/openslp/openslp/common/slp_v1message.c',
        'src/openslp/openslp/common/slp_net.c',
        'src/openslp/openslp/common/slp_xid.c',
        'src/openslp/openslp/common/slp_xmalloc.c',
        'src/openslp/openslp/common/slp_crypto.c',
        'src/openslp/openslp/common/slp_message.c',
        'src/openslp/openslp/common/slp_iface.c',
        'src/openslp/openslp/libslp/libslp_findscopes.c',
        'src/openslp/openslp/libslp/libslp_handle.c',
        'src/openslp/openslp/libslp/libslp_findsrvtypes.c',
        'src/openslp/openslp/libslp/libslp_dereg.c',
        'src/openslp/openslp/libslp/libslp_parse.c',
        'src/openslp/openslp/libslp/libslp_delattrs.c',
        'src/openslp/openslp/libslp/libslp_knownda.c',
        'src/openslp/openslp/libslp/libslp_findattrs.c',
        'src/openslp/openslp/libslp/libslp_findsrvs.c',
        'src/openslp/openslp/libslp/libslp_property.c',
        'src/openslp/openslp/libslp/libslp_reg.c',
        'src/openslp/openslp/libslp/libslp_network.c',
        'src/node_slp.cc',
        'src/api/baton.cc',
        'src/api/findsrvs.cc',
#        'src/api/findsrvtypes.cc',
        'src/api/findattrs.cc',
#        'src/api/reg.cc',
#        'src/api/dereg.cc',
#        'src/api/delattrs.cc'
      ],
      'conditions': [
        ['OS=="win"', {
		  'sources': [
			'src/openslp/openslp/common/slp_win32.c',
		  ],
 		  'defines': [
				'LIBSLP_STATIC',
            	'SLP_VERSION="2.0.0"'
		  ],
 	          'libraries': [
				'-lws2_32'
 	          ]
       }],
        ['OS=="linux"', {
		  'defines': [
				'ETCDIR="/etc"',
				'LINUX'
		  ],
        }],
        ['OS=="mac"', {
		  'defines': [
				'ETCDIR="/etc"',
				'DARWIN'
		  ],
        }]
      ],
      'libraries': [
      ]
    }
  ]
}