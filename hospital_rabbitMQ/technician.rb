require './non_admin_employee.rb'

class Technician < NonAdminEmployee

  private

  def initialize(skills: nil)
    super()
    @skills = skills
    puts "#{self}"

    # subscribing
    request_queues = @skills.map { |skill|
      skill_queue = @channel.queue("#{skill}")
      skill_queue.bind(@send_exchange, routing_key: "#{skill}")
      skill_queue
    }

    request_queues.each do |req_queue|
      req_queue.subscribe(block: false) do |delivery_info, _, body|
        @log_exchange.publish("#{@name} has received: #{body}")
        body_splitted = body.split("-")
        requesting_doctor = body_splitted[0]
        response_msg = "#{body_splitted[1]}  #{delivery_info.routing_key} - done"
        puts "Start doing sth"
        sleep(1)
        puts "End doing sth"
        @response_exchange.publish(response_msg, routing_key: requesting_doctor)
        @log_exchange.publish("#{@name} sending: #{response_msg}")
      end
    end
  end


  public

  def to_s
    "Technician: #{@name} , skills: #{@skills}"
  end

end

begin
#Start running , Usage: First arg is name , and second/third are skills
  t = Technician.new(skills: ARGV)
  loop do

  end

rescue Interrupt => _e
  t.close
end
