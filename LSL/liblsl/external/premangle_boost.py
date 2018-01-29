#!/usr/bin/env/python
import os
def process(loc,forward=True):
    for dir,subdirs,names in os.walk(loc):
        for name in names:
            if name.endswith('.hpp') or name.endswith('.h') or name.endswith('.cpp') or name.endswith('.ipp') or name.endswith('.inl'):
                path = os.path.join(dir, name)
                print '  ',path
                text = open(path,'r').read()
                if forward:
                    text = text.replace('boost','lslboost')
                else:
                    text = text.replace('lslboost','boost')             
                open(path,'w').write(text)
                
#process('./src',False)
#exit()
                
if os.path.exists('boost') and os.path.exists('libs'):
    print 'renaming directories...'
    # first rename the directories 
    os.rename('boost','lslboost')    # (note: if you're getting an error here you're probably not in the right directory or don't have the appropriate rights)
    os.rename('libs','src')
    print 'mangling symbols...'
    # then recursively rename all occurrences of boost to lslboost
    process('./lslboost')
    process('./src')
elif os.path.exists('lslboost') and os.path.exists('src'):
    print 'target directories already exist; assuming they are already processed.'
else:
    print 'neither the source nor target directories exist in this location; exiting.'
    exit()

# now generate redirect headers
redirect_headers = [
    'asio.hpp',
    'atomic.hpp',
    'bimap.hpp',
    'bind.hpp',
    'chrono.hpp',
    'config.hpp',
    'circular_buffer.hpp',
    'cstdint.hpp',
    'enable_shared_from_this.hpp',
    'filesystem.hpp',
    'format.hpp',
    'function.hpp',
    'intrusive_ptr.hpp',
    'lexical_cast.hpp',
    'noncopyable.hpp',
    'random.hpp',
    'scoped_ptr.hpp',
    'shared_ptr.hpp',
    'smart_ptr.hpp',
    'static_assert.hpp',
    'type_traits.hpp',
    'thread.hpp',
    'variant.hpp',
    'version.hpp',
    'algorithm/string.hpp',
    'archive/archive_exception.hpp',
    'archive/basic_archive.hpp',
    'archive/basic_binary_iarchive.hpp',
    'archive/basic_binary_iprimitive.hpp',
    'archive/basic_binary_oarchive.hpp',
    'archive/basic_binary_oprimitive.hpp',
    'archive/shared_ptr_helper.hpp',
    'archive/detail/polymorphic_iarchive_dispatch.hpp',
    'archive/detail/polymorphic_iarchive_impl.hpp',
    'archive/detail/polymorphic_iarchive_route.hpp',
    'archive/detail/polymorphic_oarchive_dispatch.hpp',
    'archive/detail/polymorphic_oarchive_impl.hpp',
    'archive/detail/polymorphic_oarchive_route.hpp',
    'archive/impl/archive_pointer_iserializer.ipp',
    'archive/impl/archive_pointer_oserializer.ipp',
    'archive/impl/archive_serializer_map.ipp',
    'archive/impl/basic_binary_iarchive.ipp',
    'archive/impl/basic_binary_iprimitive.ipp',
    'archive/impl/basic_binary_oarchive.ipp',
    'archive/impl/basic_binary_oprimitive.ipp',
    'asio/basic_socket.hpp',
    'asio/deadline_timer.hpp',
    'asio/io_service.hpp',
    'asio/stream_socket_service.hpp',
    'asio/streambuf.hpp',
    'asio/detail/array.hpp',
    'asio/detail/config.hpp',
    'asio/detail/throw_error.hpp',
    'asio/ip/tcp.hpp',
    'asio/ip/udp.hpp',
    'chrono/system_clocks.hpp',
    'container/flat_set.hpp',
    'date_time/time_duration.hpp',
    'detail/scoped_enum_emulation.hpp',
    'detail/endian.hpp',
    'functional/hash.hpp',
    'integer/endian.hpp',
    'lockfree/spsc_queue.hpp',
    'math/fpclassify.hpp',
    'program_options/config.hpp',
    'program_options/detail/convert.hpp',
    'property_tree/ini_parser.hpp',
    'property_tree/ptree.hpp',
    'serialization/split_member.hpp',
    'serialization/string.hpp',
    'spirit/home/support/detail/endian/endian.hpp',
    'spirit/home/support/detail/math/fpclassify.hpp',
    'system/system_error.hpp',
    'thread/once.hpp',
    'thread/thread.hpp',
    'thread/condition_variable.hpp',
    'thread/mutex.hpp',
    'type_traits/is_arithmetic.hpp',
    'type_traits/is_floating_point.hpp',
    'type_traits/is_integral.hpp',
    'type_traits/is_signed.hpp',
    'type_traits/is_unsigned.hpp',
    'utility/base_from_member.hpp',
    'utility/enable_if.hpp',
    'uuid/uuid.hpp',
    'uuid/uuid_generators.hpp',
    'uuid/uuid_io.hpp']

print 'generating redirect headers...'
for hdrpath in redirect_headers:
    # ensure that the directory exists
    catpath = 'boost/' + hdrpath
    print '  ' + catpath
    dirname = os.path.dirname(catpath)
    if not os.path.exists(dirname):
        os.makedirs(dirname)
    # create file
    with open(catpath,'w') as f:
        f.write('#include <lslboost/' + hdrpath + '>\n\n')
        f.write('#ifndef LSLBOOST_NAMESPACE_DECLARED\n')
        f.write('#define LSLBOOST_NAMESPACE_DECLARED\n')
        f.write('namespace lslboost { }; namespace boost = lslboost;\n');
        f.write('#endif\n')
print 'done.'
