require "securerandom"
class Technician

    private

    def initialize(skills = nil, name = nil)
        set_skills(skills)
        set_name(name)
    end

    def set_skills(skills)
        if skills.nil?
            raise ArgumentError("No skills specified for a technician")
        elsif skills.length <=2
            @skills = []
            skills.each do |skill|
                @skills << skill unless (!skill.class == String)
            end
            @skills.freeze
        else
            raise ArgumentError.new("Cannot have a technician with more than 2 skills!")
        end
    end

    def set_name(name)
        @name = name if name.class == String
        @name = name || SecureRandom.hex
    end

    public

    def to_s
        "Technician: #{@name} , skills: #{@skills}"
    end

    def run
        #Receive loop
        loop do
            sleep(1)
            puts "Running..."
        end
    end
    
end


#Start running , Usage: First arg is name , and second/third are skills
technician = Technician.new(skills: [ARGV[1],ARGV[2]], name: ARGV[0])
puts technician.to_s
technician.run

