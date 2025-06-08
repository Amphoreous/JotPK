// Loading Screen
window.addEventListener('load', () => {
    const loadingScreen = document.getElementById('loading-screen');
    setTimeout(() => {
        loadingScreen.style.opacity = '0';
        setTimeout(() => {
            loadingScreen.style.display = 'none';
        }, 500);
    }, 3000);
});

// Navbar Scroll Effect
window.addEventListener('scroll', () => {
    const navbar = document.getElementById('navbar');
    if (window.scrollY > 100) {
        navbar.classList.add('scrolled');
    } else {
        navbar.classList.remove('scrolled');
    }
});

// Mobile Menu Toggle
const hamburger = document.getElementById('hamburger');
const navMenu = document.getElementById('nav-menu');

hamburger.addEventListener('click', () => {
    hamburger.classList.toggle('active');
    navMenu.classList.toggle('active');
});

// Close mobile menu when clicking a link
document.querySelectorAll('.nav-link').forEach(link => {
    link.addEventListener('click', () => {
        hamburger.classList.remove('active');
        navMenu.classList.remove('active');
    });
});

// Smooth Scrolling for Anchor Links
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    });
});

// Scroll Reveal Animation
const observerOptions = {
    threshold: 0.1,
    rootMargin: '0px 0px -50px 0px'
};

const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.classList.add('revealed');
        }
    });
}, observerOptions);

// Add scroll reveal to elements
document.addEventListener('DOMContentLoaded', () => {
    const revealElements = document.querySelectorAll('.feature-card, .team-member, .feature-item');
    revealElements.forEach(el => {
        el.classList.add('scroll-reveal');
        observer.observe(el);
    });
    
    // Fetch actual file size from GitHub
    fetchGameFileSize();
});

// Parallax Effect for Hero Background
window.addEventListener('scroll', () => {
    const scrolled = window.pageYOffset;
    const heroBackground = document.querySelector('.hero-background');
    if (heroBackground) {
        heroBackground.style.transform = `translateY(${scrolled * 0.5}px)`;
    }
});

// Video Play Button Functionality
document.querySelector('.video-overlay')?.addEventListener('click', () => {
    const iframe = document.querySelector('.video-wrapper iframe');
    if (iframe) {
        const src = iframe.src;
        iframe.src = src.replace('autoplay=0', 'autoplay=1');
    }
});

// Typewriter Effect for Hero Title
function typeWriter(element, text, speed = 100) {
    let i = 0;
    element.innerHTML = '';
    
    function type() {
        if (i < text.length) {
            element.innerHTML += text.charAt(i);
            i++;
            setTimeout(type, speed);
        }
    }
    type();
}

// Initialize Animations
document.addEventListener('DOMContentLoaded', () => {
    // Add stagger animation to feature cards
    const featureCards = document.querySelectorAll('.feature-card');
    featureCards.forEach((card, index) => {
        card.style.animationDelay = `${index * 0.1}s`;
    });
    
    // Add stagger animation to team members
    const teamMembers = document.querySelectorAll('.team-member');
    teamMembers.forEach((member, index) => {
        member.style.animationDelay = `${index * 0.2}s`;
    });
});

// Particle Effect for Hero Section
function createParticles() {
    const hero = document.querySelector('.hero');
    const particlesContainer = document.createElement('div');
    particlesContainer.className = 'particles';
    particlesContainer.style.cssText = `
        position: absolute;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        pointer-events: none;
        z-index: 1;
    `;
    
    for (let i = 0; i < 50; i++) {
        const particle = document.createElement('div');
        particle.style.cssText = `
            position: absolute;
            width: 2px;
            height: 2px;
            background: rgba(212, 165, 116, 0.3);
            border-radius: 50%;
            animation: float ${Math.random() * 3 + 2}s ease-in-out infinite;
            left: ${Math.random() * 100}%;
            top: ${Math.random() * 100}%;
            animation-delay: ${Math.random() * 2}s;
        `;
        particlesContainer.appendChild(particle);
    }
    
    hero.appendChild(particlesContainer);
}

// Initialize particles after loading
setTimeout(createParticles, 3500);

// Download Button Click Tracking
document.querySelector('.btn-download')?.addEventListener('click', () => {
    // Add any analytics tracking here
    console.log('Game download initiated');
});

// Easter Egg: Konami Code
let konamiCode = [];
const sequence = ['ArrowUp', 'ArrowUp', 'ArrowDown', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'ArrowLeft', 'ArrowRight', 'KeyB', 'KeyA'];

document.addEventListener('keydown', (e) => {
    konamiCode.push(e.code);
    if (konamiCode.length > sequence.length) {
        konamiCode.shift();
    }
    
    if (konamiCode.join(',') === sequence.join(',')) {
        // Easter egg activated!
        document.body.style.filter = 'sepia(100%) hue-rotate(45deg)';
        setTimeout(() => {
            document.body.style.filter = '';
        }, 3000);
        console.log('🤠 Yeehaw! You found the secret code, partner!');
    }
});

// Enhanced file size fetch with loading state
async function fetchGameFileSize() {
    const fileSizeElement = document.querySelector('.file-size');
    
    try {
        // Show loading state
        if (fileSizeElement) {
            fileSizeElement.textContent = '(Loading...)';
        }
        
        const response = await fetch('https://api.github.com/repos/Amphoreous/JotPK/releases/latest');
        const data = await response.json();
        
        const gameAsset = data.assets.find(asset => 
            asset.name.includes('.zip') || asset.name.includes('JourneyOfThePrairieKing')
        );
        
        if (gameAsset && fileSizeElement) {
            const sizeInMB = (gameAsset.size / (1024 * 1024)).toFixed(1);
            fileSizeElement.textContent = `(${sizeInMB}MB)`;
            
            // Add a small animation when size loads
            fileSizeElement.style.opacity = '0';
            setTimeout(() => {
                fileSizeElement.style.opacity = '1';
                fileSizeElement.style.transition = 'opacity 0.3s ease';
            }, 100);
        }
    } catch (error) {
        console.error('Error fetching file size:', error);
        // Restore default size if fetch fails
        if (fileSizeElement) {
            fileSizeElement.textContent = '(~45MB)';
        }
    }
}