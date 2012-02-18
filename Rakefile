ENV['BUILD_DIR'] ||= "./build"

ENV['CFLAGS'] = "-DHAVE_CONFIG_H -I./vendor/include #{ENV['CFLAGS'].to_s}"
ENV['CXXFLAGS'] = "-DHAVE_CONFIG_H -I./vendor/include #{ENV['CXXFLAGS'].to_s}"
ENV['LIBS'] = "#{ENV['LIBS'].to_s} -lz -lcrypto -lgdsl -lsqlite3 -lpthread -ldisasm"

if ENV['OS'] =~ /Windows/
  ENV['LIBS'] += " -ldbghelp -lwininet -lws2_32"
end

PATHS = {
  :SOURCE => ["src", "vendor/breakpad"],
  :GTEST  => ["test/qt"],
  :TEST_SUPPORT => ["test/support"]
}

task :build => "build:release"

namespace :build do
  desc "Bootstrap the build environment"
  task :bootstrap do
    qmake_makefile "retropia", collect_sources(PATHS[:SOURCE]), ENV['BUILD_DIR'], :QT => "webkit"
  end

  desc "Make a debug build"
  task :debug => [:bootstrap] do
    make "-f Makefile.retropia debug"
  end
  
  desc "Make a release build"
  task :release => [:bootstrap] do
    make "-f Makefile.retropia release"
  end
end

task :gtest => "gtest:all"
namespace :gtest do
  gtest_runner = File.join(ENV['BUILD_DIR'], "gtest_runner", "gtest_runner.cc")

  desc "Bootstrap the gtest environment"
  task :bootstrap do
    generate_gtest_runner(gtest_runner)
  end

  desc "Run all gtests"
  task :all => [:bootstrap] do 
    sources = collect_sources([gtest_runner, PATHS[:GTEST], PATHS[:TEST_SUPPORT], PATHS[:SOURCE]].flatten)
    
    qmake_makefile "gtest_runner", sources, ENV['BUILD_DIR'], :QT => "webkit", 
                                  :LIBS => "-lgtest -lgmock",
                                  :CONFIG => "debug"

    make "-f Makefile.gtest_runner debug"
    sh File.join(ENV['BUILD_DIR'], "gtest_runner", "debug", "bin", "gtest_runner")
  end

  desc "Run gtests whose test path matches a given pathname [pattern]"
  task :path, [:pattern] => [:bootstrap] do |t, args|
    tests = collect_sources(PATHS[:GTEST], :pattern => /#{args.pattern}/)
    if tests[:sources].empty? && tests[:headers].empty?
      puts "No tests matching #{args.pattern}"
    else
      sources = collect_sources([gtest_runner, tests[:sources], tests[:headers], PATHS[:TEST_SUPPORT], PATHS[:SOURCE]].flatten)
    
      qmake_makefile "gtest_runner", sources, ENV['BUILD_DIR'], :QT => "webkit", 
                                                                :LIBS => "-lgtest -lgmock",
                                                                :CONFIG => "debug"

      make "-f Makefile.gtest_runner debug"
      sh File.join(ENV['BUILD_DIR'], "gtest_runner", "debug", "bin", "gtest_runner")
    end
  end
end

def generate_gtest_runner(outfile)

  mkdir_p File.dirname(outfile)

  File.open(outfile, "w") do |io|
    io << <<-EOS
    #include <gtest/gtest.h>
    #include <gmock/gmock.h>
    #include "ts_setup.h"

    int main(int argc, char **argv)
    {
      int rc;

      R_TS_SETUP_LOGGER
      
      testing::InitGoogleMock(&argc, argv);

      rc = RUN_ALL_TESTS();

      R_TS_TEARDOWN_LOGGER

      return rc;
    }
    EOS
  end
end

def qmake_makefile(target, sources, build_dir, opts = {})

  opts = { :TARGET => target, :MAKEFILE => "Makefile.#{target}" }.merge(opts)

  [:Debug, :Release].each do |build|
    opts["#{build.to_s}:OBJECTS_DIR"] = File.join(build_dir, target.to_s, build.to_s.downcase, "obj")
    opts["#{build.to_s}:MOC_DIR"] = File.join(build_dir, target.to_s, build.to_s.downcase, "obj")
    opts["#{build.to_s}:DESTDIR"] = File.join(build_dir, target.to_s, build.to_s.downcase, "bin")
  end
  
  flagstring = "-Wnone -Wlogic"
  optstring  = qmake_optstring(
          opts,
          :QMAKE_CFLAGS => ENV['CFLAGS'],
          :QMAKE_CXXFLAGS => ENV['CXXFLAGS'],
          :QMAKE_LFLAGS => ENV['LDFLAGS'],
          :LIBS => ENV['LIBS'],
          :SOURCES => sources[:sources],
          :HEADERS => sources[:headers],
          :INCLUDEPATH => sources[:dirs],
          :DEPENDPATH  => sources[:dirs]
        )

  qmake "#{flagstring} -after #{optstring}"
end

def qmake_optstring(*hashes)
  hashes.map { |h|  
    h.reject { |k, v| 
      v.nil? || v.empty? 
    }.map { |k, v| 
      "#{k.to_s}+=\"#{[v].flatten.join(' ')}\""
    }
  }.flatten.join(" ")
end

def qmake(args)
  @@qmake ||= qmake_executable
  
  sh "#{@@qmake} #{args.to_s}"
end

def qmake_executable
  if ENV['QTDIR'] 
    File.join(ENV['QTDIR'], "bin", "qmake")
  else
    qmake_from_path = %x[which qmake].chomp
    if $?.exitstatus == 0
      qmake_from_path
    else
      raise "QTDIR was not set and qmake is not in your PATH!"
    end 
  end
end

def make(args)
  sh "make #{args.to_s}"
end

def files(path, &block)
  FileList.new(File.directory?(path) ? "#{path}/**/*.*" : path, &block)
end

def directories(path, &block)
  FileList.new("#{path}/**/", &block)
end

def collect_sources(*args)
  params = args.last.is_a?(Hash) ? args.pop : {}
  paths = args

  entrypoints = []

  collect = lambda do |hash, path|
    files(path) do |fs|
      fs.egrep(/(void|int)\s+main\s*\(/) { |f, n, l| entrypoints << f }

      headers = fs.dup.exclude { |f| !(f =~ /\.(h|hh|hxx|hpp)$/) || (params.include?(:pattern) && !(f =~ params[:pattern])) }
      sources = fs.dup.exclude { |f| !(f =~ /\.(c|cc|cxx|cpp)$/) || (params.include?(:pattern) && !(f =~ params[:pattern])) }
      (hash[:headers] ||= []).concat(headers)
      (hash[:sources] ||= []).concat(sources)
      (hash[:dirs] ||= []).concat(directories(path)) if (headers.size + sources.size) > 0
    end

    hash
  end

  sources = paths.flatten.inject({}, &collect)

  if entrypoints.size > 1
    sources[:headers] = sources[:headers] - entrypoints + [entrypoints.first]
    sources[:sources] = sources[:sources] - entrypoints + [entrypoints.first]
  end

  sources
end

PROJECT_CEEDLING_ROOT = "vendor/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/rakefile.rb"

task :default => %w[ test:all gtest:all build:release ]