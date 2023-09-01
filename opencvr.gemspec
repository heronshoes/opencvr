# frozen_string_literal: true

require_relative 'lib/opencvr/version'

Gem::Specification.new do |spec|
  spec.name = 'opencvr'
  spec.version = OpenCVR::VERSION
  spec.authors = ['wagavulin']
  spec.email = ['xxx@xxx.com']

  spec.summary = 'A Ruby binding of OpenCV'
  spec.description = 'opencvr is a ruby extension library which provides the wrapper of OpenCV.'
  spec.homepage = 'https://github.com/wagavulin/opencvr'
  spec.license = 'Apache License 2.0'
  spec.required_ruby_version = '>= 3.0'

  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['source_code_uri'] = spec.homepage
  spec.metadata['changelog_uri'] = "#{spec.homepage}/CHANGELOG.md"

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject do |f|
      (f == __FILE__) ||
        f.match(%r{\A(?:(?:bin|test|spec|features)/|\.(?:git|travis|circleci)|appveyor)})
    end
  end
  spec.bindir = 'exe'
  spec.executables = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']

  spec.add_dependency 'numo-narray'

  # Development dependency has gone to the Gemfile (rubygems/bundler#7237)

  spec.metadata['rubygems_mfa_required'] = 'true'
end
